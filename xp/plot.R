library(ggplot2)
library(reshape2)
library(scales)
library(dplyr)

entropy <- scan("mit.ent", quiet = TRUE)
ent_mat <- matrix(entropy, byrow=T, nrow=3327, ncol=3327)
threshold <- 1.6

# melted_ent_mat <- melt(ent_mat) %>%
#   mutate(val_trimmed = case_when(
#     value <= threshold ~ threshold,
#     T ~ value
#   )) 
# 
# ggplot(melted_ent_mat, aes(x = Var1, y = Var2, fill = val_trimmed)) +
#   geom_raster() +
#   scale_fill_gradient(low="yellow", high="dark green")

ent_mat[ent_mat < threshold] <- threshold
melted_ent_mat <- melt(ent_mat)
# data_below_thresh <- subset(melted_ent_mat, melted_ent_mat$value<=threshold)
# data_above_thresh <- subset(melted_ent_mat, melted_ent_mat$value>threshold)

# ggplot() +
#   geom_raster(data=data_above_thresh, aes(x = Var1, y = Var2, fill=value)) +
#   geom_raster(data=data_below_thresh, aes(x = Var1, y = Var2), fill='yellow') +
#   scale_fill_gradient(low="yellow", high="dark green") +
#   labs(x="Mitochondrion", y="Mitochondrion") +
#   theme_bw()

ggplot() +
  geom_raster(data=melted_ent_mat, aes(x = Var1, y = Var2, fill=value)) +
  scale_fill_gradient(low="yellow", high="dark green") +
  labs(x="Mitochondrion", y="Mitochondrion") +
  theme_bw()
