library(ggplot2)
library(reshape2)
library(scales)
# library(dplyr)
library(viridis)

# n_row_col <- 113
# entropy <- scan("ent_Chondrichthyes.tsv", quiet = TRUE)
# ent_mat <- matrix(entropy,
#                   byrow = T,
#                   nrow = n_row_col,
# ncol = n_row_col)


# entropy <- read.table("ent_Chondrichthyes.tsv", header = TRUE)
# entropy_mat <- as.matrix(entropy)
# threshold <- 0.5
# # entropy_mat[entropy_mat < threshold] <- threshold
# melted_ent_mat <- melt(entropy_mat)
#
# ggplot(melted_ent_mat, aes(x = Var2, y = Var1)) +
#   geom_raster(aes(fill=value)) +
#   scale_fill_gradient(low="yellow", high="dark green") +
#   theme_bw() + theme(axis.text.x=element_text(angle=90, vjust=0.3))


rearrange <- read.table("rearrange_count.tsv", header = TRUE)
rearrange_mat <- as.matrix(rearrange)
melted_rearrange_mat <- melt(rearrange_mat)


ggplot(melted_rearrange_mat, aes(x = Var2, y = Var1)) +
  geom_raster(aes(fill = value)) +
  scale_fill_gradient(low="white", high="black") +
  # scale_fill_viridis(direction = 1) +
  theme_bw() +
  ggtitle("Number of rearrangements in Chondrichthyes") +
  theme(axis.title.x = element_blank(), axis.title.y = element_blank(),
        axis.text.x = element_text(angle = 90, vjust = 0.3))


# ggplot(melted_ent_mat, aes(x = value)) +
#   geom_density(fill = 'blue')


# melted_ent_mat <- melt(ent_mat) %>%
#   mutate(val_trimmed = case_when(
#     value <= threshold ~ threshold,
#     T ~ value
#   ))
#
# ggplot(melted_ent_mat, aes(x = Var1, y = Var2, fill = val_trimmed)) +
#   geom_raster() +
#   scale_fill_gradient(low="yellow", high="dark green")

# ent_mat[ent_mat < threshold] <- threshold
# melted_ent_mat <- melt(ent_mat)
# data_below_thresh <- subset(melted_ent_mat, melted_ent_mat$value<=threshold)
# data_above_thresh <- subset(melted_ent_mat, melted_ent_mat$value>threshold)

# ggplot() +
#   geom_raster(data=data_above_thresh, aes(x = Var1, y = Var2, fill=value)) +
#   geom_raster(data=data_below_thresh, aes(x = Var1, y = Var2), fill='yellow') +
#   scale_fill_gradient(low="yellow", high="dark green") +
#   labs(x="Mitochondrion", y="Mitochondrion") +
#   theme_bw()

# ggplot() +
#   geom_raster(data=melted_ent_mat, aes(x = Var1, y = Var2, fill=value)) +
#   scale_fill_gradient(low="yellow", high="dark green") +
#   labs(x="Mitochondrion", y="Mitochondrion") +
#   theme_bw()
