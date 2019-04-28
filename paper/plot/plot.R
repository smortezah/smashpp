library(ggplot2)
library(reshape2)

filters <- 0
filters_scatter <- 1

if (filters == 1) {
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
  index<-1:999
  blackman <- read.table("blackman.fil")
  hamming <- read.table("hamming.fil")
  hann <- read.table("hann.fil")
  nuttall <- read.table("nuttall.fil")
  rectangular <- read.table("rectangular.fil")
  sine <- read.table("sine.fil")
  triangular <- read.table("triangular.fil")
  welch <- read.table("welch.fil")
  main <- read.table("0.prf")

  df <-
    data.frame(
      Samples = index,
      Blackman = blackman$V1,
      Hamming = hamming$V1,
      Hann = hann$V1,
      Nuttall = nuttall$V1,
      Rectangular = rectangular$V1,
      Sine = sine$V1,
      Triangular = triangular$V1,
      Welch = welch$V1,
      Main = main$V1[1:999]
    )
  df <- melt(df, id.vars = "Samples")
  df$name <-
    as.factor(rep(
      c(
        "Blackman",
        "Hamming",
        "Hann",
        "Nuttall",
        "Rectangular",
        "Sine",
        "Triangular",
        "Welch",
        "Main"
      ),
      each = length(index)
    ))

  ggplot(df, aes(x = Samples, y = value)) +
    geom_line(aes(color = variable)) +
    facet_wrap( ~ name, nrow = 4) +
    ylab("Value") +
    theme_bw() +
    theme(legend.position = "none")
}
