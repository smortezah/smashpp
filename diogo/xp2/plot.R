library(ggplot2)
library(reshape2)
library(scales)
# library(dplyr)
library(ggcorrplot)
library(corrplot)
library(factoextra)

plot_rearrange <- TRUE
plot_nrc <- FALSE

if (plot_rearrange) {
  rearrange <-
    read.table("rearrange_count_Chondrichthyes.tsv", header = TRUE)
  rearrange_mat <- as.matrix(rearrange)
  header <- colnames(rearrange_mat)
  melted_rearrange_mat <- melt(rearrange_mat)
  
  top_row_mat <- as.matrix(rearrange_mat['NC_028344', ])
  top_row_mat <- subset(top_row_mat, top_row_mat != 0)
  top_row_mat <- top_row_mat[order(top_row_mat[,1], decreasing = TRUE)]
  # colnames(top_row_mat) <- c("NC_028344")
  # melted_top_row_mat <- melt(top_row_mat)

  # corr <- round(cor(rearrange), 1)
  # res.dist <- get_dist(rearrange, method = "euclidean")
  # fviz_dist(res.dist, lab_size = 8)
  # res.hc <- hclust(res.dist, method = "ward.D2")
  # plot(res.hc, cex = 0.5)
  # corrplot(
  #   rearrange_mat[,1:40],
  #   is.corr = FALSE,
  #   type = "upper",
  #   method = "color",
  #   tl.col = "black",
  #   outline = TRUE,
  #   addgrid.col = "white",
  #   # cl.align = "l",
  # ) #+
  # ggplot(melted_rearrange_mat, aes(x = Var2, y = Var1)) +
  #   geom_tile(aes(fill = value), colour = "white") +
  ggplot() +
    # geom_tile(
    #   melted_rearrange_mat,
    #   mapping = aes(x = Var2, y = Var1, fill = value),
    #   colour = "white"
    # ) +
    geom_tile(
      melted_top_row_mat,
      mapping = aes(x = Var1, y = Var2, fill = value),
      colour = "white",
      # na.rm = TRUE
    ) +
    # geom_raster(aes(fill = value)) +
    coord_fixed() +
    # scale_fill_gradient2(
    #   low = "white",
    #   mid = "lightblue",
    #   high = "yellow",
    #   midpoint = 63,
    #   space = "Lab"
    # ) +
    # ggtitle("Chondrichthyes") +
    scale_fill_gradientn(
      # labels = c("1", str(seq(10,130,10))),
      # breaks = c(1, seq(10,130,10)),
      guide = guide_colorbar(
        title = "No.\nrearrangements\n(Chondrichthyes)",
        title.position = "top",
        title.vjust = 1,
        # label.position = "left"
      ),
      colours = hcl.colors(5, palette = "spectral", rev = TRUE),
      # values = rescale(c(0, 30, 40, 50, 90, 110, 125)),
      limits = c(1, 126),
      na.value = "white"
    ) +
    # scale_x_discrete(position = "top") +
    # scale_x_discrete(limit = c(rev(header))) +
    # theme_bw() +
    theme(
      axis.title.x = element_blank(),
      axis.title.y = element_blank(),
      axis.text.x = element_text(angle = 90, vjust = 0.3),
      # legend.direction = "horizontal",
      # legend.background = element_rect(fill = "transparent"),
      # legend.title = element_text(color = "white"),
      # legend.text = element_text(color = "white"),
      # legend.key.size = unit(0.5, "cm"),
      legend.justification = c(1, 1),
      legend.position = c(1, 1),
      # legend.key.height = unit(1.25, "cm"),
      legend.key.height = unit(6.5, "cm"),
      # Makes sense in scale=3
      # legend.text.align = 1
    )
  
  # ggsave("rearrange_count_Chondrichthyes.pdf", scale = 3)
} else if (plot_nrc) {
  ent <- read.table("ent_Chondrichthyes.tsv", header = TRUE)
  ent_mat <- as.matrix(ent)
  corr <- round(cor(ent), 1)
  # p.mat <- cor_pmat(ent)
  # ggcorrplot(corr,
  #            hc.order = TRUE,
  #            type = "lower",
  #            outline.col = "white") #+
  # corrplot(corr, type = "upper", order = "hclust", method = "color", tl.col = "black", addrect = 5, col=hcl.colors(9, palette = "spectral", rev = TRUE)) #+
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