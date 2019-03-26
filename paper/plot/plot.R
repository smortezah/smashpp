library(ggplot2)
library(reshape2)

N <- 100
hann <- function(n) {0.5 - 0.5 * cos(2 * pi * n / N)}
hamming <- function(n) {0.54 - 0.46 * cos(2 * pi * n / N)}
blackman <- function(n) {0.42 - 0.5 * cos(2 * pi * n / N) + 
    0.08 * cos(4 * pi * n / N)}
nuttall <- function(n) {0.36 - 0.49 * cos(2 * pi * n / N) + 
    0.14 * cos(4 * pi * n / N) - 0.01 * cos(6 * pi * n / N)}


# % void triangular();  // Bartlett window
# % void welch();
# % void sine();
# % void nuttall();


x <- seq(0, N)
df <-
  data.frame(
    Samples = x,
    hann = hann(x),
    hamming = hamming(x),
    blackman = blackman(x),
    nuttall = nuttall(x)
  )
df <- melt(df, id.vars = "Samples")
df$name <-
  as.factor(rep(c("Hann", "Hamming", "Blackman", "Nuttall"), each = length(x)))

ggplot(df, aes(x = Samples, y = value)) +
  geom_area(aes(fill = variable)) +
  facet_wrap( ~ name) +
  theme_bw()
