library(ggplot2)
library(reshape2)
library(scales)

entropy <- scan("mit.ent", quiet = TRUE)
ent_mat <- matrix(entropy, byrow=T, nrow=3327, ncol=3327)
threshold <- 1.2
ent_mat[ent_mat > threshold] <- threshold
melted_ent_mat <- melt(ent_mat)

ggplot() +
  geom_raster(data=melted_ent_mat, aes(x = Var1, y = Var2, fill=value)) +
  labs(x="Mitochondrion", y="Mitochondrion") +
  # scale_fill_manual(name = 'fill', values = setNames(c(melted_ent_mat$value,'yellow'),c(T, F))) +
  scale_fill_gradient(low="yellow", high="dark green") +
  theme_bw()

