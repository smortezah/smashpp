library(ggplot2)
library(reshape2)

data_in <- scan("mit.ent", quiet = TRUE)
mat <- matrix(data_in, byrow=T, nrow=3327, ncol=3327)
melted_mat <- melt(mat)

ggplot(melted_mat, aes(x = Var1, y = Var2)) + 
  geom_raster(aes(fill=value)) +
  scale_fill_gradient(low="blue", high="red")