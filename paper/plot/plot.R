library(ggplot2)
library(ggpubr)
library(reshape2)
theme_set(theme_bw())

compare_smash <- T
filters <- 0
filters_scatter <- 0
simil <- 0

if (compare_smash) {
  mut.smashpp.ir0 <- read.table('0.RefMut_smash.TarMut_smash.fil')
  mut.smashpp.ir1 <- read.table('1.RefMut_smash.TarMut_smash.fil')
  mut.smash.ir0 <- read.table('TarMut_smash.inf.fil')
  mut.smash.ir1 <- read.table('TarMut_smash_inv.inf.fil')
  thresh <- 1.55
  thresh_color <- rgb(192/255, 0, 0)
  line_thickness <- 0.75

  a <- ggplot(data=mut.smashpp.ir0, aes(x=10*seq(1, 100001), y=mut.smashpp.ir0$V1)) +
    geom_line(size = line_thickness) +
    ylim(0.15, 2.1) +
    geom_hline(yintercept = thresh, color = thresh_color, size = line_thickness) +
    annotate("text", x = 15000, y = 1.45, label = "thr", color = thresh_color) +
    ylab('Information content (bpb)') +
    theme(axis.title.x=element_blank(),
          axis.text.x=element_blank(),
          axis.ticks.x=element_blank())

  b <- ggplot(data=mut.smashpp.ir1, aes(x=10*seq(1, 100001), y=mut.smashpp.ir1$V1)) +
    geom_line(size = line_thickness) +
    ylim(0.15, 2.1) +
    geom_hline(yintercept = thresh, color = thresh_color, size = line_thickness) +
    annotate("text", x = 15000, y = 1.45, label = "thr", color = thresh_color) +
    xlab('Base position') +
    ylab('Information content (bpb)')
  
  c <- ggplot(data=mut.smash.ir0, aes(x=100*seq(1, 9901), y=mut.smash.ir0$V2)) +
    geom_line(size = line_thickness) +
    geom_hline(yintercept = thresh, color = thresh_color, size = line_thickness) +
    annotate("text", x = 15000, y = 1.45, label = "thr", color = thresh_color) +
    ylim(0.15, 2.1) +
    ylab('Information content (bpb)') +
    theme(axis.title.x=element_blank(),
          axis.text.x=element_blank(),
          axis.ticks.x=element_blank())
  
  d <- ggplot(data=mut.smash.ir1, aes(x=100*seq(1, 9901), y=mut.smash.ir1$V2)) +
    geom_line(size = line_thickness) +
    geom_hline(yintercept = thresh, color = thresh_color, size = line_thickness) +
    annotate("text", x = 15000, y = 1.45, label = "thr", color = thresh_color) +
    ylim(0.15, 2.1) +
    xlab('Base position') +
    ylab('Information content (bpb)')

  ggarrange(a, c, b, d, nrow = 2, ncol = 2)
  ggsave("compare_smash_mut.bmp", scale = 0.9)
} else if (filters == 1) {
  N <- 100
  hann <- function(n) {
    0.5 - 0.5 * cos(2 * pi * n / N)
  }
  hamming <- function(n) {
    0.54 - 0.46 * cos(2 * pi * n / N)
  }
  blackman <- function(n) {
    0.42 - 0.5 * cos(2 * pi * n / N) +
      0.08 * cos(4 * pi * n / N)
  }
  nuttall <- function(n) {
    0.36 - 0.49 * cos(2 * pi * n / N) +
      0.14 * cos(4 * pi * n / N) - 0.01 * cos(6 * pi * n / N)
  }
  welch <- function(n) {
    1 - ((n - N / 2) / (N / 2)) ^ 2
  }
  sine <- function(n) {
    sin(pi * n / N)
  }
  triangular <- function(n) {
    1 - abs((n - N / 2) / (N / 2))
  }
  rectangular <- function(n) {
    1
  }
  
  
  x <- seq(0, N)
  df <-
    data.frame(
      Samples = x,
      hann = hann(x),
      hamming = hamming(x),
      blackman = blackman(x),
      nuttall = nuttall(x),
      welch = welch(x),
      sine = sine(x),
      triangular = triangular(x),
      rectangular = rectangular(x)
    )
  df <- melt(df, id.vars = "Samples")
  df$name <-
    as.factor(rep(
      c(
        "Hann",
        "Hamming",
        "Blackman",
        "Nuttall",
        "Welch",
        "Sine",
        "Triangular",
        "Rectangular"
      ),
      each = length(x)
    ))
  
  ggplot(df, aes(x = Samples, y = value)) +
    geom_area(aes(color = variable, fill = variable)) +
    facet_wrap( ~ name, nrow = 2) +
    ylab("Amplitude") +
    theme_bw() +
    theme(legend.position = "none")
} else if (filters_scatter == 1) {
  index<-1:1000
  blackman <- read.table("blackman.fil")
  hamming <- read.table("hamming.fil")
  hann <- read.table("hann.fil")
  nuttall <- read.table("nuttall.fil")
  rectangular <- read.table("rectangular.fil")
  sine <- read.table("sine.fil")
  triangular <- read.table("triangular.fil")
  welch <- read.table("welch.fil")
  main <- read.table("main.prf")

  df <-
    data.frame(
      Samples = index,
      Main = main$V1,
      Rectangular = rectangular$V1,
      Hamming = hamming$V1,
      Hann = hann$V1,
      Blackman = blackman$V1,
      Triangular = triangular$V1,
      Welch = welch$V1,
      Sine = sine$V1,
      Nuttall = nuttall$V1
    )
  df <- melt(df, id.vars = "Samples")
  df$name <-
    as.factor(rep(
      c(
        "Main",
        "Rectangular",
        "Hamming",
        "Hann",
        "Blackman",
        "Triangular",
        "Welch",
        "Sine",
        "Nuttall"
      ),
      each = length(index)
    ))
  df$name <-
    factor(
      df$name,
      c(
        "Main",
        "Rectangular",
        "Hamming",
        "Hann",
        "Blackman",
        "Triangular",
        "Welch",
        "Sine",
        "Nuttall"
      )
    )
  
  ggplot(df, aes(x = Samples, y = value)) +
    geom_line(aes(color = variable)) +
    facet_wrap(~name, ncol = 2, strip.position = "top"
               # , scales = "free_y"
               ) +
    geom_ribbon(data=subset(df, value<=1.5),aes(ymin = value, ymax = 1.5), fill = "gray", alpha = .5) +
    xlab("Base") +
    ylab("Entropy") +
    theme_bw() + 
    theme(legend.position = "none")
} else if (simil == 1) {
  # index<-1:1000
  hann <- read.table("hann.fil")
  main <- read.table("main.prf")
  
  # df <-
  #   data.frame(
  #     Samples = index,
  #     # Main = main$V,
  #     Hann = hann$V1
  #   )
  # # df <- melt(df, id.vars = "Samples")
  # # df$name <-
  # #   as.factor(rep(
  # #     c(
  # #       "Main",
  # #       "Hann"
  # #     ),
  # #     each = length(index)
  # #   ))
  # # df$name <-
  # #   factor(
  # #     df$name,
  # #     c(
  # #       "Main",
  # #       "Hann"
  # #     )
  # #   )
  # 
  # plt1 <- ggplot(df, aes(x = Samples, y = Hann)) +
  #   geom_line(aes(color = "red")) +
  #   # geom_line(aes(x = Samples, y = 1.5)) +
  #   # facet_wrap(~name, nrow = 2, strip.position = "top"
  #   #            # , scales = "free_y"
  #   # ) +
  #   geom_ribbon(data=subset(df, value<=1.5),aes(ymin = value, ymax = 1.5), fill = "gray", alpha = .5) +
  #   xlab("Base") +
  #   ylab("Entropy") +
  #   theme_bw() + 
  #   theme(legend.position = "none")
  # 
  # plt1
  
  
  df <- data.frame(x = 1:1000, y = hann$V1)
  plt1 <- ggplot(df, aes(x))
  
  # h + geom_ribbon(aes(ymin=0, ymax=y))
  # h + geom_area(aes(y = y))
  
  plt1 +
    geom_ribbon(data = df[df$y<=1.5,], aes(ymin = y, ymax = 1.5)) +
    geom_line(aes(y = y))
  # +
  #   scale_fill_manual(values=c("green"), name="fill")
}

