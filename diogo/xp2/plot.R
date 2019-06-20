library(ggplot2)
library(reshape2)
library(scales)
# library(dplyr)

rearrange <-
  read.table("rearrange_count_Chondrichthyes.tsv", header = TRUE)
rearrange_mat <- as.matrix(rearrange)
melted_rearrange_mat <- melt(rearrange_mat)


ggplot(melted_rearrange_mat, aes(x = Var2, y = Var1)) +
  geom_raster(aes(fill = value)) +
  # scale_fill_gradient2(low = "red", mid = "white",
  #                      high = "blue", midpoint = 65, space = "Lab") +
  scale_fill_gradientn(
    colours = hcl.colors(9, palette = "spectral", rev = TRUE),
    values = rescale(c(0, 40, 80, 125)),
    limits = c(0, 125)
  ) +
  theme_bw() +
  ggtitle("Number of rearrangements in Chondrichthyes") +
  scale_y_discrete(position = "right") +
  theme(
    axis.title.x = element_blank(),
    axis.title.y = element_blank(),
    axis.text.x = element_text(angle = 90, vjust = 0.3),
    legend.position = c(0.08, 0.6),
    legend.background = element_rect(fill = "transparent"),
    legend.title = element_text(color = "white"),
    legend.text = element_text(color = "white"),
    legend.key.size = unit(1.0, "cm"),
    legend.key.width = unit(0.5, "cm")
  )

# ggsave("rearrange_count_Chondrichthyes.pdf", scale = 3)


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