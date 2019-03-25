library(ggplot2)

hann <- function(n) {
  if (!(length(n) == 1 && (n == round(n)) && (n > 0)))
    stop("hanning: n has to be an integer > 0")
  
  if (n == 1)
    c = 1
  else {
    n = n - 1
    c = 0.5 - 0.5 * cos(2 * pi * (0:n) / n)
  }
  c
}

hann(5)
