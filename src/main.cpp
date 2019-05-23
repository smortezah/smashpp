// Smash++
// Morteza Hosseini    seyedmorteza@ua.pt
// Copyright (C) 2018-2019, IEETA, University of Aveiro, Portugal.
//    ____                      _
//   / ___| _ __ ___   __ _ ___| |__    _     _
//   \___ \| '_ ` _ \ / _` / __| '_ \ _| |_ _| |_
//    ___) | | | | | | (_| \__ \ | | |_   _|_   _|
//   |____/|_| |_| |_|\__,_|___/_| |_| |_|   |_|

//                 ```.```
//           `-/oydmmmmmmmmdho:`
//          `smNNMMMMMMMMMNNMMNd+`
//          :hNNNmmdhhmmddddmmNNNh`
//         sNNmy+/::--:::--::/ohNN/
//        .NMNs/:----.-..-----:/hNy
//        :NMm+::---........--:/sNd`
//        -NNd/://+oo+/-:/+o+///sNh
//        `dNh/+yyhdddy:/dmmhhyooNo
//        `sds:+syydhho.-yddhys++h-
//        `+y+:---:::::.-:///:::/s-
//         :+/:--...-:-.--:-..-:/+-
//         .//::----:+oooo/:---:/:`
//          .//:::://+yyys//::///`
//           .++//+sssyyyssyo+++-
//            /+++//:/osso//+++/
//            :ooo+/::://///+oo.
//          `:/ossssoo+oooosssoso.
//         `sm++ossyyyyyyyyyso+smh`
//      ``.smdo++ossssyyyysoo++hNNh/.`
//  `.-:/+odmNmysoooooossoooshmNNNmsso/.`
// /+++++++odNNNNmdhhyssyhdmNNNNNdyooo+++/:
// ++++++++++ymNNNNNNmddmmmNNNNmhssooo+++++
// +++++++++++ohmmmmmmmNmddmmmdyysoooo+++++
// +++++++++++oossyyyhmmhhhhhhyysoooo++++++
// o+++++++++++++oosyhddyhhhyyssooo++++++++

// MMMMNMMNNMNNNNNNNNNMNNNNmmmmmNNNNNNNMNNN
// MMMMMMMNNMNNNNNNmmdddmmNNNmmmNNNMNNNNNNN
// MMMMMMMNNMMNmhso+////++shdNNNNNNMNmNNNNN
// MMMMMMMMNMMds+:-..``..-:/sdNNNNNNmNNMNNN
// MMMMMMMMMMmy+:.```  ``.-:+sdNMNMMMNMMMMN
// MMMMMMMMMMh+/:-.``````.-:/oymMMMMMNMMMMN
// MMMMMMMMMNsohhyo:``` `/yhhdddMMMMMMMMMMN
// MMMMMMMMMNyy/...`.```````.:sdNMMMMMMMMMM
// MMMMMMMMMNo/ooso/::://::+ssysmMMMMMMMMMM
// MMMMMMMMmdo+/.``.:/:+o+-..:osddMMMMMMMMM
// MMMMMMMMNyo/.```-/:-:oo:---:odhMMMMMMMMM
// MMMMMMMMM+y:.```.-:..//-```-+dyMMMMMMMMM
// MMMMMMMMMmm+-.`.://--+syo:-:oNMMMMMMMMMM
// MMMMMMMMMMNs/::+o:.```-o+//+yNMMMMMMMMMM
// MMMMMMMMMMMh//:://-..-/o//+sdMMMMMMMMMMM
// MMMMMMMMMMMNs+//-..``..-+sshmMMMMMMMMMMM
// MMMMMMMMMMMMsooo-```.`.-shhydMMMMMMMMMMM
// MMMMMMMMMMMMy::oo:---::+hs+odmddmMMMMMMN
// MMMMMMMMms+-o:..-/+//+//:::+yy:-::/sdmNN
// MMMMNhs:````.:.````````...-/o-...-.-:-:s

// +++/+:-://+/:---.---:-.-:::hyyyyyyyy..-/
// oo+/:::-:-/+:/++ooooo+++/:/ysosyhsss/:-.
// so+oo+o/:-:oyhhhhhyyyhhysyyys+/+oso//:o+
// sso///+sosyyyhhhhhyyyyyssssssss+::::/:/o
// /+/////oyssyhhhhdmmNNNNNmmmmmmdhs+/-----
// ossooooyhydNmdhhdddddhddddmmdddmds/:::-/
// /++/oohddmNdo+///++++++++++//::oddyooo+/
// ++o+/shdmmh/::-::///////::::----:ddsoo++
// o++//syhdho/:://++++++/////:::---ody+//+
// sooo/shhys+///+++++++++////////::/ho:/os
// ys/-/yhhs/////////++///////::::://s+so+o
// oo+o:sys+://///++oooooo/+++//::-:/o:oooo
// ++//s+++::/+osyhmmmmmhyo+ydddho////-/+++
// osso+sy/::/+sssyhhdddhs//ydmdhyy+/os::``
// ssso:++//::///+ossssso/:-/oyyo+//+osoo/-
// ++/://o+////++///+os+/-::-://////--::++:
// yssyy+++//++++++osyy+//+/:/+///////+oooo
// ssyyyso///+ooosyhyyyhdyhyo+os+++/+sssoo+
// ooooooo/++osssyhhhhyhhhhysossssoooos+ooo
// o+oooso++ossyyhhdddddhdhhhhhyssoooo+//+o
// ::+shyds+osyhhhhyyyyyyysoooyyyddhy+:.-:-
// ::+ydNm+osyyhhhhyyyyhdhysssyhmdmmmmmh+//
// hhyhNNd+osyhhhddhhyyyyyssyyodmmmmmNNmNmd
// hsymNNmsosyhhhyhhddddddhyssyNNdNmmmNNNNN
// yydmmmmhssyhddhhhdddhhyyssyNNNmdNNNNNNNN
// hhdmmmmmhhyyhhhhhdddhhyyyhNNNNNmmNNNNNNN
// hddmmNNmmddhhhhhhddhhhhhmNNNNNMNNNNNNNNN

// #define __STDC_FORMAT_MACROS
// #if defined(_MSC_VER)
// #include <io.h>
// #else
// #include <unistd.h>
// #endif

#include <iostream>
#include <chrono>
#include <iomanip>      // setw, setprecision
#include <thread>
#include "par.hpp"
#include "fcm.hpp"
#include "filter.hpp"
#include "container.hpp"
#include "time.hpp"
#include "naming.hpp"
#include "string.hpp"
#include "segment.hpp"
#include "vizpaint.hpp"
using namespace smashpp;

struct OutRowAux {
  PosRow pos2;
  PosRow pos1;
  std::vector<PosRow> pos3;
  OutRowAux() = default;
  OutRowAux(PosRow pos2_, PosRow pos1_) : pos2(pos2_), pos1(pos1_) {}
};

void write_pos_file_impl(const std::vector<OutRowAux>& out_aux) {
  const auto ref{out_aux.front().pos1.ref};
  const auto tar{out_aux.front().pos1.tar};
  const auto pos_file_name{gen_name(ref, tar, Format::position)};
  std::ofstream pos_file(pos_file_name);

  // Head
  pos_file << POS_HDR << '\t' << file_name(ref) << '\t'
           << std::to_string(file_size(ref)) << '\t' << file_name(tar) << '\t'
           << std::to_string(file_size(tar)) << '\n';

  // Body
  for (auto row : out_aux) {
    // Left hand side
    if (row.pos2.beg_pos == 0 && row.pos2.end_pos == 0 && row.pos2.ent == 0 &&
        row.pos2.self_ent == 0)
      pos_file << DBLANK << '\t' << DBLANK << '\t' << DBLANK << '\t' << DBLANK
               << '\t';
    else
      pos_file << row.pos2.beg_pos << '\t' << row.pos2.end_pos << '\t'
               << row.pos2.ent << '\t' << row.pos2.self_ent << '\t';

    // Right hand side
    if (row.pos3.size() == 1) {
      if (row.pos3.front().run_num == 0)
        pos_file << row.pos1.beg_pos + row.pos3.front().beg_pos << '\t'
                 << row.pos1.beg_pos + row.pos3.front().end_pos << '\t';
      else if (row.pos3.front().run_num == 1)
        pos_file << row.pos1.beg_pos + row.pos3.front().end_pos << '\t'
                 << row.pos1.beg_pos + row.pos3.front().beg_pos << '\t';

      pos_file << row.pos3.front().ent << '\t' << row.pos3.front().self_ent
               << '\n';
    } else {
      if (row.pos1.run_num == 0)
        pos_file << row.pos1.beg_pos << '\t' << row.pos1.end_pos << '\t';
      else if (row.pos1.run_num == 1)
        pos_file << row.pos1.end_pos << '\t' << row.pos1.beg_pos << '\t';

      pos_file << row.pos1.ent << '\t' << row.pos1.self_ent << '\n';
    }
  }

  pos_file.close();
}

void make_write_pos_pair(const std::vector<PosRow>& left,
                         const std::vector<PosRow>& right1,
                         const std::vector<PosRow>& right3) {
  std::vector<OutRowAux> out_aux{std::vector<OutRowAux>()};

  if (left.empty()) {
    for (const auto& row : right1) out_aux.push_back(OutRowAux(PosRow(), row));
  } else {
    for (const auto& row_left : left) {
      for (const auto& row_right1 : right1) {
        const auto seg_name{gen_name(row_right1.run_num, row_right1.ref,
                                     row_right1.tar, Format::segment) +
                            std::to_string(row_right1.seg_num)};
        if (row_left.ref == seg_name) {
          out_aux.push_back(OutRowAux(row_left, row_right1));
          break;
        }
      }
    }

    for (const auto& row_right3 : right3) {
      for (auto row_out_aux : out_aux) {
        const auto seg_name{gen_name(row_out_aux.pos2.run_num,
                                     row_out_aux.pos2.ref, row_out_aux.pos2.tar,
                                     Format::segment) +
                            std::to_string(row_out_aux.pos2.seg_num)};
        if (row_right3.ref == seg_name) {
          row_out_aux.pos3.push_back(row_right3);
          break;
        }
      }
    }
  }

  if (!out_aux.empty()) write_pos_file_impl(out_aux);
}

void write_pos_file(const std::vector<PosRow>& pos_out) {
  std::vector<PosRow> left, right1, right3;
  for (const auto& row : pos_out) {
    if (row.round == 2) left.push_back(PosRow(row));
    if (row.round == 1) right1.push_back(PosRow(row));
    if (row.round == 3) right3.push_back(PosRow(row));
  }

  make_write_pos_pair(left, right1, right3);
}

uint64_t run_round(std::unique_ptr<Param>& par, uint8_t round, uint8_t run_num,
                   std::vector<PosRow>& pos_out, uint64_t& current_pos_row) {
  par->ID = run_num;
  par->refName = file_name(par->ref);
  par->tarName = file_name(par->tar);
  auto models = std::make_unique<FCM>(par);

  if (round == 1 && run_num == 0)
    std::cerr << bold(
        "====[ REGULAR MODE ]==================================\n");
  else if (round == 1 && run_num == 1)
    std::cerr << bold(
        "====[ INVERTED MODE ]=================================\n");

  // Make all IRs consistent
  for (auto& ref_model : models->rMs) {
    ref_model.ir = run_num;
    if (ref_model.child) ref_model.child->ir = run_num;
  }
  for (auto& tar_model : models->tMs) {
    tar_model.ir = run_num;
    if (tar_model.child) tar_model.child->ir = run_num;
  }

  // Build models and Compress
  models->store(par, round);
  models->compress(par, round);

  // Filter and segment
  auto filter = std::make_unique<Filter>(par);
  if (!par->manThresh) par->thresh = static_cast<float>(models->aveEnt);
  filter->smooth_seg(pos_out, par, round, current_pos_row);

  if (filter->nSegs == 0) {
    // if (round == 2) {
    //   pos_out.push_back(
    //       PosRow(0, 0, 0.0, 0.0, run_num, par->ref, par->tar, 0, round));
    // }
    std::cerr << '\n';
    return 0;  // continue;
  }
  filter->extract_seg(pos_out, round, run_num, par->ref);

  // Ref-free compress
  if (!par->noRedun) {
    if (par->verbose) {
      if (round == 3) std::cerr << "    ";
      std::cerr << "[+] Reference-free compression of the segment"
                << (filter->nSegs == 1 ? "" : "s") << '\n';
    } else {
      if (round == 1) par->message = "[+] Ref-free compression of ";
    }

    const auto seg{gen_name(par->ID, par->ref, par->tar, Format::segment)};
    models->selfEnt.reserve(filter->nSegs);
    for (uint64_t i = 0; i != filter->nSegs; ++i) {
      if (!par->verbose && round == 1)
        std::cerr << "\r" << par->message << "segment " << i + 1 << " ...";

      par->seq = seg + std::to_string(i);
      models->self_compress(par, i, round);
    }

    models->aggregate_slf_ent(pos_out, round, run_num, par->ref, par->noRedun);
    if (!par->verbose && round == 1) {
      std::cerr << "\r" << par->message
                << (filter->nSegs == 1 ? "the segment " : "all segments ")
                << "done.         " << '\n';
    }
  }

  current_pos_row += filter->nSegs;
  return filter->nSegs;
}

void remove_temp_seg(std::unique_ptr<Param>& par, uint64_t seg_num) {
  const auto seg{gen_name(par->ID, par->ref, par->tar, Format::segment)};

  for (uint64_t i = 0; i != seg_num; ++i)
    if (!par->saveAll && !par->saveSegment)
      remove((seg + std::to_string(i)).c_str());
}

void remove_temp_seq(std::unique_ptr<Param>& par) {
  if (par->refType == FileType::fasta || par->refType == FileType::fastq) {
    if (!par->saveSeq) {
      remove(par->ref.c_str());
    } else {
      const std::string seq_name = file_name_no_ext(par->ref) + ".seq";
      rename(par->ref.c_str(), seq_name.c_str());
    }
    rename((par->ref + LBL_BAK).c_str(), par->ref.c_str());
  }

  if (par->tarType == FileType::fasta || par->tarType == FileType::fastq) {
    if (!par->saveSeq) {
      remove(par->tar.c_str());
    } else {
      const std::string seq_name = file_name_no_ext(par->tar) + ".seq";
      rename(par->tar.c_str(), seq_name.c_str());
    }
    rename((par->tar + LBL_BAK).c_str(), par->tar.c_str());
  }
}

void prepare_data(std::unique_ptr<Param>& par) {
  if (par->refType == FileType::seq && par->tarType == FileType::seq) return;

  std::cerr << bold("====[ PREPARE DATA ]==================================\n");
  
  // FASTA/FASTQ to seq
  const auto convert_to_seq = [](std::string in, std::string out,
                                 const FileType& type) {
    std::string msg = "[+] " + italic(in) + " (FAST";
    if (type == FileType::fasta)
      msg += "A";
    else if (type == FileType::fastq)
      msg += "Q";
    msg += ") -> " + italic(out) + " (seq) ";
    std::cerr << msg << "...";
    rename(in.c_str(), (in + LBL_BAK).c_str());
    to_seq(in + LBL_BAK, in, type);
    std::cerr << "\r" << msg << "finished.\n";
  };

  const std::string ref_seq = file_name_no_ext(par->refName) + ".seq";
  const std::string tar_seq = file_name_no_ext(par->tarName) + ".seq";

  if (par->refType == FileType::fasta)
    convert_to_seq(par->refName, ref_seq, FileType::fasta);
  else if (par->refType == FileType::fastq)
    convert_to_seq(par->refName, ref_seq, FileType::fastq);
  else if (par->refType != FileType::seq)
    error("\"" + par->refName + "\" has unknown format.");

  if (par->tarType == FileType::fasta)
    convert_to_seq(par->tarName, tar_seq, FileType::fasta);
  else if (par->tarType == FileType::fastq)
    convert_to_seq(par->tarName, tar_seq, FileType::fastq);
  else if (par->tarType != FileType::seq)
    error("\"" + par->tarName + "\" has unknown format.");

  std::cerr << '\n';
}

void run(std::unique_ptr<Param>& par) {
  std::string ref_round1 = par->ref;
  std::string tar_round1 = par->tar;
  auto filter = std::make_unique<Filter>();
  std::vector<PosRow> pos_out;
  uint64_t current_pos_row = 0;

  // FASTA/FASTQ to seq, if applicable
  prepare_data(par);

  // Round 1
  for (uint8_t run_num = 0; run_num != 2; ++run_num) {
    auto num_seg_round1 = run_round(par, 1, run_num, pos_out, current_pos_row);

    // Round 2: old ref = new tar & old tar segments = new refs
    if (num_seg_round1 != 0) {
      if (par->verbose) {
        par->message = italic("Repeat above process for each segment");
        std::cerr << '\n' << par->message << '\n';
      } else {
        par->message = "[+] Repeating above process for ";
      }

      const auto name_seg_round1{
          gen_name(par->ID, ref_round1, tar_round1, Format::segment)};
      std::string tar_round2 = par->tar = par->ref;

      for (uint64_t i = 0; i != num_seg_round1; ++i) {
        if (!par->verbose)
          std::cerr << "\r" << par->message << "segment " << i + 1 << " ...";

        std::string ref_round2 = par->ref = name_seg_round1 + std::to_string(i);
        auto num_seg_round2 =
            run_round(par, 2, run_num, pos_out, current_pos_row);
        if (par->verbose) std::cerr << '\n';

        if (num_seg_round2 != 0) {
          // Round 3
          if (par->deep) {
            if (par->verbose)
              std::cerr << "    " << italic("Deep compression") << '\n';

            const auto name_seg_round2{
                gen_name(par->ID, ref_round2, tar_round2, Format::segment)};
            par->tar = ref_round2;

            for (uint64_t j = 0; j != num_seg_round2; ++j) {
              par->ref = name_seg_round2 + std::to_string(j);
              auto num_seg_round3 =
                  run_round(par, 3, run_num, pos_out, current_pos_row);
              if (par->verbose) std::cerr << "\n";
              remove_temp_seg(par, num_seg_round3);
            }
          }  // Round 3

          par->ref = ref_round2;
          par->tar = tar_round2;
          remove_temp_seg(par, num_seg_round2);
        }
      }

      if (!par->verbose)
        std::cerr << "\r" << par->message << "all segments done.\n\n";
    }  // Round 2

    par->ref = ref_round1;
    par->tar = tar_round1;
    remove_temp_seg(par, num_seg_round1);
    // remove_temp_seq(par);
  }  // Round 1

  remove_temp_seq(par);
  if (!pos_out.empty()) write_pos_file(pos_out);
}

int main(int argc, char* argv[]) {
  try {
    const auto t0{now()};

    if (has(argv, argv + argc, std::string("-viz"))) {
      auto vizpar = std::make_unique<VizParam>();
      vizpar->parse(argc, argv);
      auto paint = std::make_unique<VizPaint>();
      paint->plot(vizpar);
    } else {
      auto par = std::make_unique<Param>();
      par->parse(argc, argv);
      run(par);
    }

    const auto t1{now()};
    std::cerr << "Total time: " << hms(t1 - t0);
  } catch (std::exception& e) {
    std::cerr << e.what();
  } catch (...) {
    return EXIT_FAILURE;
  }

  return 0;
}