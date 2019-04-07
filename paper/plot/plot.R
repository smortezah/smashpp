library(ggplot2)
library(reshape2)

N <- 100
hann <- function(n) {0.5 - 0.5 * cos(2 * pi * n / N)}
hamming <- function(n) {0.54 - 0.46 * cos(2 * pi * n / N)}
blackman <- function(n) {0.42 - 0.5 * cos(2 * pi * n / N) + 
    0.08 * cos(4 * pi * n / N)}
nuttall <- function(n) {0.36 - 0.49 * cos(2 * pi * n / N) + 
    0.14 * cos(4 * pi * n / N) - 0.01 * cos(6 * pi * n / N)}
welch <- function(n) {1 - ((n - N/2)/(N/2))^2}
sine <- function(n) {sin(pi * n / N)}
triangular <- function(n) {1 - abs((n - N/2)/(N/2))}
rectangular <- function(n) {1}


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
df$name <- as.factor(rep(c("Hann", "Hamming", "Blackman", "Nuttall", "Welch", 
                           "Sine", "Triangular", "Rectangular"), each = length(x)))

ggplot(df, aes(x = Samples, y = value)) +
  geom_area(aes(color=variable, fill = variable)) +
  facet_wrap( ~ name, nrow = 2) +
  ylab("Amplitude") +
  theme_bw() + 
  theme(legend.position = "none")
