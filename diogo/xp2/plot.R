library(ggplot2)
library(reshape2)
library(scales)
# library(dplyr)
library(ggcorrplot)
library(corrplot)

plot_rearrange <- FALSE
plot_nrc <- TRUE

if (plot_rearrange) {
  rearrange <-
    read.table("rearrange_count_Chondrichthyes.tsv", header = TRUE)
  rearrange_mat <- as.matrix(rearrange)
  melted_rearrange_mat <- melt(rearrange_mat)
  
  ggplot(melted_rearrange_mat, aes(x = Var2, y = Var1)) +
    geom_raster(aes(fill = value)) +
    # scale_fill_gradient2(low = "red", mid = "white",
    #                      high = "blue", midpoint = 65, space = "Lab") +
    ggtitle("Chondrichthyes") +
    scale_fill_gradientn(
      guide = guide_colorbar(
        title = "Number of rearrangements",
        title.position = "top",
        title.hjust = 0.5
      ),
      colours = hcl.colors(9, palette = "spectral", rev = TRUE),
      # values = rescale(c(0, 30, 40, 50, 90, 110, 125)),
      # limits = c(0, 125)
    ) +
    scale_y_discrete(position = "right") +
    theme_bw() +
    theme(
      axis.title.x = element_blank(),
      axis.title.y = element_blank(),
      axis.text.x = element_text(angle = 90, vjust = 0.3),
      legend.direction = "horizontal",
      legend.justification = c(0, 0),
      legend.position = c(0, 0.8),
      legend.background = element_rect(fill = "transparent"),
      legend.title = element_text(color = "white"),
      legend.text = element_text(color = "white"),
      legend.key.size = unit(0.5, "cm"),
      legend.key.width = unit(1.25, "cm")
    )
  
  # ggsave("rearrange_count_Chondrichthyes.pdf", scale = 3)
} else if (plot_nrc) {
  ent <- read.table("ent_Chondrichthyes.tsv", header = TRUE)
  # ent_mat <- as.matrix(ent)
  corr <- round(cor(ent), 1)
  p.mat <- cor_pmat(ent)
  # ggcorrplot(corr,
  #            hc.order = TRUE,
  #            type = "lower",
  #            outline.col = "white") #+
  corrplot(corr, type = "upper", order = "hclust", method = "color", tl.col = "black", outline = TRUE) #+
    # ggtitle("Chondrichthyes") +
    # # scale_fill_gradientn(
    # #   guide = guide_colorbar(
    # #     title = "Number of rearrangements",
    # #     title.position = "top",
    # #     title.hjust = 0.5
    # #   ),
    # #   colours = hcl.colors(3, palette = "spectral", rev = TRUE),
    # #   # values = rescale(c(0, 30, 40, 50, 90, 110, 125)),
    # #   limits = c(-1, 1)
    # # ) +
    # # scale_y_discrete(position = "right") +
    # theme_bw() +
    # theme(
    #   axis.title.x = element_blank(),
    #   axis.title.y = element_blank(),
    #   axis.text.x = element_text(angle = 90, vjust = 0.3),
    #   # legend.direction = "horizontal",
    #   # legend.justification = c(0, 0),
    #   # legend.position = c(0, 0.8),
    #   # legend.background = element_rect(fill = "transparent"),
    #   # # legend.title = element_text(color = "white"),
    #   # # legend.text = element_text(color = "white"),
    #   # legend.key.size = unit(0.5, "cm"),
    #   # legend.key.width = unit(1.25, "cm")
    # )
} else {
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
}