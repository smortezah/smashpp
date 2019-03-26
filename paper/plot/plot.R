library(ggplot2)

N<-50
hann <- function(n) {0.5 - 0.5 * cos(2 * pi * n / N)}

ggplot(NULL, aes(c(0,N))) + 
  geom_area(stat = "function", fun = hann, fill = "red")