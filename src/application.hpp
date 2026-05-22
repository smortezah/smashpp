// SPDX-FileCopyrightText: 2018-2026 Morteza Hosseini
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SMASHPP_APPLICATION_HPP
#define SMASHPP_APPLICATION_HPP

#include <algorithm>
#include <chrono>
#include <format>
#include <future>
#include <iomanip>  // setw, setprecision
#include <iostream>
#include <iterator>
#include <memory>
#include <thread>

#include "container.hpp"
#include "exception.hpp"
#include "fcm.hpp"
#include "file.hpp"
#include "filter.hpp"
#include "memory.hpp"
#include "naming.hpp"
#include "output.hpp"
#include "par.hpp"
#include "segment.hpp"
#include "string.hpp"
#include "time.hpp"
#include "vizpaint.hpp"

namespace smashpp {
constexpr auto NO_SEGMENTS_GUIDANCE =
    "no similar regions passed the current segmentation settings. For chromosome-scale or more "
    "divergent genomes, try increasing -th/--threshold, reducing -m/--min-segment-size, using "
    "-fs/--filter-scale L, or lowering -d/--sampling-step.";

class application {
 public:
  application() = default;
  void exe(int, char**);

 private:
  void run(Param&);
  auto run_mode(Param&, uint8_t, const std::string&, const std::string&) -> std::vector<PosRow>;
  auto run_modes_parallel(Param&, const std::string&, const std::string&) -> std::vector<PosRow>;
  auto run_round(Param&, uint8_t, uint8_t, std::vector<PosRow>&, uint64_t&) -> uint64_t;

  void prepare_data(Param&);
  void remove_temp_seg(Param&, uint64_t);
  void remove_temp_seq(Param&);
};

class info {
 private:
  uint8_t lblWidth = 20;
  uint8_t colWidth = 8;
  uint8_t tblWidth = 52;

 public:
  info() = default;
  void show(const Param&);

 private:
  void show_ref_FCM(const Param&) const;
  void show_ref_STMM(const Param&) const;
  void show_tar_FCM(const Param&) const;
  void show_tar_STMM(const Param&) const;
  void show_filter(const Param&) const;
  void show_file(const Param&) const;

  void rule(uint8_t, std::string&&) const;
  void toprule() const;
  void midrule() const;
  void botrule() const;
  void label(std::string) const;
  void header(std::string) const;
  void info_FCM(std::vector<MMPar> const&, char) const;
  void info_STMM(std::vector<MMPar> const&, char) const;
  void info_filter(const Param&, char) const;
  void info_file(const Param&, char) const;
};

void application::exe(int argc, char* argv[]) {
  if (has(argv, argv + argc, std::string("-viz")) || has(argv, argv + argc, std::string("viz"))) {
    auto vizpar = std::make_unique<VizParam>();
    vizpar->parse(argc, argv);
    auto paint = std::make_unique<VizPaint>();
    paint->plot(vizpar);
  } else {
    Param par;
    par.parse(argc, argv);
    run(par);
  }
}

void application::run(Param& par) {
  std::vector<PosRow> pos_out;

  // FASTA/FASTQ to seq, if applicable
  prepare_data(par);

  std::string ref_round1 = par.ref;
  std::string tar_round1 = par.tar;
  const auto concurrent_modes = (par.nthr > 1 && !par.verbose) ? 2ull : 1ull;
  enforce_memory_budget(par, concurrent_modes);

  const auto append_rows = [](auto& out, auto& rows) {
    out.insert(std::end(out), std::make_move_iterator(std::begin(rows)),
               std::make_move_iterator(std::end(rows)));
  };

  if (par.nthr > 1 && !par.verbose) {
    auto rows = run_modes_parallel(par, ref_round1, tar_round1);
    append_rows(pos_out, rows);
  } else {
    for (uint8_t run_num = 0; run_num < 2; ++run_num) {
      auto rows = run_mode(par, run_num, ref_round1, tar_round1);
      append_rows(pos_out, rows);
    }
  }

  if (pos_out.empty() && !par.quiet) {
    warning(std::string(NO_SEGMENTS_GUIDANCE));
  }

  PositionFile pos_file;
  pos_file.param_list = par.param_list;
  pos_file.info->ref = file_name(par.original_ref.empty() ? par.ref : par.original_ref);
  pos_file.info->ref_size = file_size(par.ref);
  pos_file.info->tar = file_name(par.original_tar.empty() ? par.tar : par.original_tar);
  pos_file.info->tar_size = file_size(par.tar);
  pos_file.dump(pos_out, par.asym_region, par.format);

  remove_temp_seq(par);
}

std::vector<PosRow> application::run_modes_parallel(Param& par, const std::string& ref_round1,
                                                    const std::string& tar_round1) {
  auto regular = par.clone();
  auto inverted = par.clone();
  const auto regular_threads = static_cast<uint8_t>(std::max<uint32_t>(1, par.nthr / 2));
  const auto inverted_threads =
      static_cast<uint8_t>(std::max<uint32_t>(1, par.nthr - regular_threads));

  regular.nthr = regular_threads;
  inverted.nthr = inverted_threads;
  regular.quiet = true;
  inverted.quiet = true;

  const auto message = std::format(
      "[+] Running regular and inverted modes in parallel ({} + {} "
      "threads) ",
      regular_threads, inverted_threads);
  std::cerr << bold("====[ REGULAR + INVERTED MODE ]=======================\n");
  std::cerr << message << "...";

  auto regular_future = std::async(
      std::launch::async, [this, mode_par = std::move(regular), ref_round1, tar_round1]() mutable {
        return run_mode(mode_par, 0, ref_round1, tar_round1);
      });
  auto inverted_future = std::async(
      std::launch::async, [this, mode_par = std::move(inverted), ref_round1, tar_round1]() mutable {
        return run_mode(mode_par, 1, ref_round1, tar_round1);
      });

  auto regular_rows = regular_future.get();
  auto inverted_rows = inverted_future.get();

  std::vector<PosRow> pos_out;
  pos_out.reserve(regular_rows.size() + inverted_rows.size());
  pos_out.insert(std::end(pos_out), std::make_move_iterator(std::begin(regular_rows)),
                 std::make_move_iterator(std::end(regular_rows)));
  pos_out.insert(std::end(pos_out), std::make_move_iterator(std::begin(inverted_rows)),
                 std::make_move_iterator(std::end(inverted_rows)));

  std::cerr << "\r" << message << "done.         \n\n";
  return pos_out;
}

std::vector<PosRow> application::run_mode(Param& par, uint8_t run_num,
                                          const std::string& ref_round1,
                                          const std::string& tar_round1) {
  std::vector<PosRow> pos_out;
  uint64_t current_pos_row = 0;

  auto num_seg_round1 = run_round(par, 1, run_num, pos_out, current_pos_row);

  // Round 2: old ref = new tar & old tar segments = new refs
  if (num_seg_round1 != 0) {
    if (par.verbose) {
      par.message = italic("Repeat above process for each segment");
      if (!par.quiet) {
        std::cerr << '\n' << par.message << '\n';
      }
    } else {
      par.message = "[+] Repeating above process for ";
    }

    const auto name_seg_round1{gen_name(par.ID, ref_round1, tar_round1, Format::segment)};
    std::string tar_round2 = par.tar = par.ref;

    for (uint64_t i = 0; i < num_seg_round1; ++i) {
      if (!par.quiet && !par.verbose) {
        std::cerr << "\r" << par.message << "segment " << i + 1 << " ... ";
      }

      std::string ref_round2 = par.ref = std::format("{}{}", name_seg_round1, i);

      auto num_seg_round2 = run_round(par, 2, run_num, pos_out, current_pos_row);
      if (!par.quiet && par.verbose) {
        std::cerr << '\n';
      }

      if (num_seg_round2 != 0) {
        // Round 3
        if (par.deep) {
          if (!par.quiet && par.verbose) {
            std::cerr << "    " << italic("Deep compression") << '\n';
          }

          const auto name_seg_round2{gen_name(par.ID, ref_round2, tar_round2, Format::segment)};
          par.tar = ref_round2;

          for (uint64_t j = 0; j < num_seg_round2; ++j) {
            par.ref = std::format("{}{}", name_seg_round2, j);
            auto num_seg_round3 = run_round(par, 3, run_num, pos_out, current_pos_row);
            if (!par.quiet && par.verbose) {
              std::cerr << "\n";
            }
            remove_temp_seg(par, num_seg_round3);
          }
        }  // Round 3

        par.ref = ref_round2;
        par.tar = tar_round2;
        remove_temp_seg(par, num_seg_round2);
      }
    }

    if (!par.quiet && !par.verbose) {
      std::cerr << "\r" << par.message << "all segments done.\n\n";
    }
  }  // Round 2

  par.ref = ref_round1;
  par.tar = tar_round1;
  remove_temp_seg(par, num_seg_round1);

  return pos_out;
}

uint64_t application::run_round(Param& par, uint8_t round, uint8_t run_num,
                                std::vector<PosRow>& pos_out, uint64_t& current_pos_row) {
  par.ID = run_num;
  par.refName = file_name(par.ref);
  par.tarName = file_name(par.tar);
  FCM models{par};

  if (!par.quiet && par.verbose && par.showInfo) {
    info{}.show(par);
    par.showInfo = false;
  }
  if (!par.quiet && round == 1 && run_num == 0) {
    std::cerr << bold("====[ REGULAR MODE ]==================================\n");
  } else if (!par.quiet && round == 1 && run_num == 1) {
    std::cerr << bold("====[ INVERTED MODE ]=================================\n");
  }

  // Make all IRs consistent
  for (auto& ref_model : models.rMs) {
    ref_model.ir = run_num;
    if (ref_model.child) {
      ref_model.child->ir = run_num;
    }
  }
  for (auto& tar_model : models.tMs) {
    tar_model.ir = run_num;
    if (tar_model.child) {
      tar_model.child->ir = run_num;
    }
  }

  // Build models and Compress
  models.store(par, round);
  models.compress(par, round);

  // Filter and segment
  Filter filter{par};
  filter.smooth_seg(pos_out, models.profileEnt, par, round, current_pos_row);

  if (filter.nSegs == 0) {
    if (!par.quiet && round == 1) {
      std::cerr << '\n';
    }
    return 0;  // continue;
  }
  const auto need_segment_files =
      par.saveAll || par.saveSegment || round == 1 || (round == 2 && par.deep);
  const auto segment_views =
      filter.extract_seg(pos_out, round, run_num, par.ref, need_segment_files);
  if (segment_views.size() != filter.nSegs) {
    error("failed to prepare segment views.");
  }

  // Ref-free compress
  if (!par.noRedun) {
    if (!par.quiet && par.verbose) {
      if (round == 3) {
        std::cerr << "    ";
      }
      std::cerr << "[+] Reference-free compression of the segment" << (filter.nSegs == 1 ? "" : "s")
                << '\n';
    } else {
      if (round == 1) {
        par.message = "[+] Ref-free compression of ";
      }
    }

    const auto seg{gen_name(par.ID, par.ref, par.tar, Format::segment)};
    models.selfEnt.resize(segment_views.size());
    for (uint64_t i = 0; i < segment_views.size(); ++i) {
      if (!par.quiet && !par.verbose && round == 1) {
        std::cerr << "\r" << par.message << "segment " << i + 1 << " ...";
      }

      par.seq = std::format("{}{}", seg, i);
      models.self_compress(par, segment_views.at(i), i, round);
    }

    models.aggregate_slf_ent(pos_out, round, run_num, par.ref, par.noRedun);
    if (!par.quiet && !par.verbose && round == 1) {
      std::cerr << "\r" << par.message << (filter.nSegs == 1 ? "the segment " : "all segments ")
                << "done.         " << '\n';
    }
  }

  current_pos_row += filter.nSegs;
  return filter.nSegs;
}

void application::prepare_data(Param& par) {
  if (par.refType == FileType::seq && par.tarType == FileType::seq) {
    return;
  }

  std::cerr << bold("====[ PREPARE DATA ]==================================\n");

  // FASTA/FASTQ to seq
  auto convert_to_seq = [](std::string in, std::string out, const FileType& type) {
    const auto type_label = (type == FileType::fasta) ? "FASTA" : "FASTQ";
    const auto msg =
        std::format("[+] {} ({}) -> {} (seq) ", italic(file_name(in)), type_label, italic(out));
    std::cerr << msg << "...";
    to_seq(in, out, type);
    std::cerr << "\r" << msg << "finished.\n";
  };

  const std::string ref_seq = std::format("{}.seq", file_name_no_ext(par.refName));
  const std::string tar_seq = std::format("{}.seq", file_name_no_ext(par.tarName));

  if (par.refType == FileType::fasta || par.refType == FileType::fastq) {
    convert_to_seq(par.ref, ref_seq, par.refType);
    par.ref = ref_seq;
    par.refName = file_name(par.ref);
    par.refType = FileType::seq;
  } else if (par.refType != FileType::seq) {
    error(std::format("\"{}\" has unknown format.", par.refName));
  }

  if (par.tarType == FileType::fasta || par.tarType == FileType::fastq) {
    convert_to_seq(par.tar, tar_seq, par.tarType);
    par.tar = tar_seq;
    par.tarName = file_name(par.tar);
    par.tarType = FileType::seq;
  } else if (par.tarType != FileType::seq) {
    error(std::format("\"{}\" has unknown format.", par.tarName));
  }

  std::cerr << '\n';
}

void application::remove_temp_seg(Param& par, uint64_t seg_num) {
  const auto seg{gen_name(par.ID, par.ref, par.tar, Format::segment)};

  for (uint64_t i = 0; i != seg_num; ++i) {
    if (!par.saveAll && !par.saveSegment) {
      remove(std::format("{}{}", seg, i).c_str());
    }
  }
}

void application::remove_temp_seq(Param& par) {
  const std::string ref_seq = std::format("{}.seq", file_name_no_ext(par.ref));
  const std::string tar_seq = std::format("{}.seq", file_name_no_ext(par.tar));

  if (par.refType == FileType::fasta || par.refType == FileType::fastq) {
    if (!par.saveSeq) {
      remove(ref_seq.c_str());
    }
  }

  if (par.tarType == FileType::fasta || par.tarType == FileType::fastq) {
    if (!par.saveSeq) {
      remove(tar_seq.c_str());
    }
  }
}

void info::show(const Param& par) {
  show_ref_FCM(par);
  show_ref_STMM(par);
  show_tar_FCM(par);
  show_tar_STMM(par);
  show_filter(par);
  show_file(par);
}

void info::show_ref_FCM(const Param& par) const {
  auto ref_FCM_row = [&](std::string lbl, char c) {
    label(lbl);
    info_FCM(par.refMs, c);
    std::cerr << '\n';
  };

  toprule();
  ref_FCM_row(bold("Ref Model           "), 'm');
  midrule();
  ref_FCM_row("Context size (k)", 'k');
  bool hasSketch{false};
  for (const auto& e : par.refMs) {
    if (e.cont == Container::sketch_8) {
      hasSketch = true;
      break;
    }
  }
  if (hasSketch) {
    ref_FCM_row("Sketch width (w)", 'w');
    ref_FCM_row("Sketch depth (d)", 'd');
  }
  ref_FCM_row("Inv. repeat  (ir)", 'i');
  ref_FCM_row("Alpha        (a)", 'a');
  ref_FCM_row("Gamma        (g)", 'g');
  botrule();  // cerr << '\n';
}

void info::show_ref_STMM(const Param& par) const {
  bool show_rstmm = false;
  for (const auto& model : par.refMs) {
    if (model.child) {
      show_rstmm = true;
      break;
    }
  }
  if (!show_rstmm) {
    return;
  }

  auto ref_STMM_row = [&](std::string lbl, char c) {
    label(lbl);
    if (c != 'h') info_STMM(par.refMs, c);
    std::cerr << '\n';
  };

  toprule();
  ref_STMM_row(bold("Ref Substituttional Model           "), 'h');
  midrule();
  ref_STMM_row("No. Subst.   (t)", 't');
  ref_STMM_row("Inv. repeat  (ir)", 'i');
  ref_STMM_row("Alpha        (a)", 'a');
  ref_STMM_row("Gamma        (g)", 'g');
  botrule();  // cerr << '\n';
}

void info::show_tar_FCM(const Param& par) const {
  auto tar_FCM_row = [&](std::string lbl, char c) {
    label(lbl);
    info_FCM(par.tarMs, c);
    std::cerr << '\n';
  };

  toprule();
  tar_FCM_row(bold("Tar Model           "), 'm');
  midrule();
  tar_FCM_row("Context size (k)", 'k');
  bool hasSketch = false;
  for (const auto& e : par.tarMs) {
    if (e.cont == Container::sketch_8) {
      hasSketch = true;
      break;
    }
  }
  if (hasSketch) {
    tar_FCM_row("Sketch width (w)", 'w');
    tar_FCM_row("Sketch depth (d)", 'd');
  }
  tar_FCM_row("Inv. repeat  (ir)", 'i');
  tar_FCM_row("Alpha        (a)", 'a');
  tar_FCM_row("Gamma        (g)", 'g');
  botrule();  // cerr << '\n';
}

void info::show_tar_STMM(const Param& par) const {
  bool show_tstmm = false;
  for (const auto& model : par.tarMs) {
    if (model.child) {
      show_tstmm = true;
      break;
    }
  }
  if (!show_tstmm) {
    return;
  }

  auto tar_STMM_row = [&](std::string lbl, char c) {
    label(lbl);
    if (c != 'h') {
      info_STMM(par.tarMs, c);
    }
    std::cerr << '\n';
  };

  toprule();
  tar_STMM_row(bold("Tar Substituttional Model           "), 'h');
  midrule();
  tar_STMM_row("No. Subst.   (t)", 't');
  tar_STMM_row("Inv. repeat  (ir)", 'i');
  tar_STMM_row("Alpha        (a)", 'a');
  tar_STMM_row("Gamma        (g)", 'g');
  botrule();  // cerr << '\n';
}

void info::show_filter(const Param& par) const {
  if (par.compress) {
    return;
  }

  auto filter_row = [&](std::string lbl, char c) {
    label(lbl);
    if (c != 'h') {
      info_filter(par, c);
    }
    std::cerr << '\n';
  };

  toprule();
  filter_row(bold("Filter & Segment"), 'h');
  midrule();
  filter_row("Window function", 'f');
  if (par.manFilterScale) {
    filter_row("Filter scale", 's');
  }
  if (!par.manFilterScale) {
    filter_row("Window size", 'w');
  }
  if (par.manThresh) {
    filter_row("Threshold", 't');
  }
  botrule();  // cerr << '\n';
}

void info::show_file(const Param& par) const {
  auto file_row = [&](std::string lbl, std::string s1, std::string s2) {
    label(lbl);
    if (s1.size() == 1) {
      info_file(par, s1.front());
      info_file(par, s2.front());
    } else {
      header(s1);
      header(s2);
    }
    std::cerr << '\n';
  };

  toprule();
  file_row(bold("File                "), bold("Name            "), bold("Size (B)"));
  midrule();
  file_row("Reference", "r", "1");
  file_row("Target", "t", "2");
  botrule();
}

void info::rule(uint8_t n, std::string&& s) const {
  for (auto i = n / s.size(); i--;) {
    std::cerr << s;
  }
  std::cerr << '\n';
}

void info::toprule() const {}

void info::midrule() const { rule(tblWidth, "="); }

void info::botrule() const { rule(tblWidth, " "); }

void info::label(std::string s) const { std::cerr << std::setw(lblWidth) << std::left << s; }

void info::header(std::string s) const { std::cerr << std::setw(2 * colWidth) << std::left << s; }

void info::info_FCM(const std::vector<MMPar>& Ms, char c) const {
  int i = 0;
  for (const auto& e : Ms) {
    std::cerr << std::setw(colWidth) << std::left;
    switch (c) {
      case 'm':
        std::cerr << ++i;
        break;
      case 'k':
        std::cerr << static_cast<int>(e.k);
        break;
      case 'w':
        std::cerr << (e.w == 0 ? "0" : std::format("2^{}", static_cast<int>(std::log2(e.w))));
        break;
      case 'd':
        std::cerr << static_cast<int>(e.d);
        break;
      case 'i':
        std::cerr << (e.ir == 0 ? "reg" : e.ir == 1 ? "inv" : "reg+inv");
        break;
      case 'a':
        std::cerr << e.alpha;
        break;
      case 'g':
        std::cerr << e.gamma;
        break;
    }
  }
}

void info::info_STMM(std::vector<MMPar> const& Ms, char c) const {
  for (const auto& e : Ms) {
    std::cerr << std::setw(colWidth) << std::left;
    if (e.child) {
      switch (c) {
        case 't':
          std::cerr << static_cast<int>(e.child->thresh);
          break;
        case 'i':
          std::cerr << (e.child->ir == 0 ? "reg" : e.child->ir == 1 ? "inv" : "reg+inv");
          break;
        case 'a':
          std::cerr << e.child->alpha;
          break;
        case 'g':
          std::cerr << e.child->gamma;
          break;
        default:
          break;
      }
    } else {
      std::cerr << '-';
    }
  }
}

void info::info_filter(const Param& par, char c) const {
  std::cerr << std::setw(colWidth) << std::left;
  switch (c) {
    case 'f':
      std::cerr << par.print_win_type();
      break;
    case 's':
      std::cerr << par.print_filter_scale();
      break;
    case 'w':
      std::cerr << par.filt_size;
      break;
    case 't':
      std::cerr << par.thresh;
      break;
    default:
      break;
  }
}

void info::info_file(const Param& par, char c) const {
  std::cerr << std::setw(2 * colWidth) << std::left;
  switch (c) {
    case '1':
      std::cerr << file_size(par.ref);
      break;
    case 'r':
      std::cerr << par.refName;
      break;
    case '2':
      std::cerr << file_size(par.tar);
      break;
    case 't':
      std::cerr << par.tarName;
      break;
    default:
      break;
  }
}
}  // namespace smashpp

#endif  // SMASHPP_APPLICATION_HPP
