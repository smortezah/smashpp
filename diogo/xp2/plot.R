library(ggplot2)
library(reshape2)
library(scales)
# library(dplyr)
library(ggpubr)
library(ggplotify)
library(factoextra)
library(cluster)
# library(biclust)
# theme_set(theme_bw())

library(devtools)
# install_github("jokergoo/ComplexHeatmap")
library(ComplexHeatmap)
library(circlize)

plot_rearrange_Chondrichthyes <- F
plot_rearrange_Mammalia <- FALSE
plot_nrc_Chondrichthyes <- T
plot_nrc_Mammalia <- FALSE
cluster_Chondrichthyes <- FALSE

if (plot_rearrange_Chondrichthyes) {
  in_file = "rearrange_count_Chondrichthyes_symmetric.tsv"
  # in_file = "rearrange_count_Chondrichthyes.tsv"
  out_file = "rearrange_count_Chondrichthyes.pdf"
  rearrange <-
    read.table(in_file, header = TRUE)
  rearrange_mat <- as.matrix(rearrange)
  header <- colnames(rearrange_mat)
  molten_rearrange_mat <- melt(rearrange_mat)
  
  # top_row_mat <- as.matrix(rearrange_mat['NC_028344', ])
  # top_row_mat <- subset(top_row_mat, top_row_mat != 0)
  # # top_row_mat <- top_row_mat[order(top_row_mat[,1], decreasing = TRUE)]
  # colnames(top_row_mat) <- c("NC_028344")
  # # melted_top_row_mat <- melt(top_row_mat)
  # 
  # # corr <- round(cor(rearrange), 1)
  # # res.dist <- get_dist(rearrange, method = "euclidean")
  # # fviz_dist(res.dist, lab_size = 8)
  # # res.hc <- hclust(res.dist, method = "ward.D2")
  # # plot(res.hc, cex = 0.5)
  # # corrplot(
  # #   rearrange_mat[,1:40],
  # #   is.corr = FALSE,
  # #   type = "upper",
  # #   method = "color",
  # #   tl.col = "black",
  # #   outline = TRUE,
  # #   addgrid.col = "white",
  # #   # cl.align = "l",
  # # ) #+
  # # ggplot(molten_rearrange_mat, aes(x = Var2, y = Var1)) +
  # #   geom_tile(aes(fill = value), colour = "white") +
  # ggplot() +
  #   geom_tile(
  #     molten_rearrange_mat,
  #     mapping = aes(x = Var2, y = Var1, fill = value),
  #     colour = "white"
  #   ) +
  #   coord_fixed() +
  #   # scale_fill_gradient2(
  #   #   low = "white",
  #   #   mid = "lightblue",
  #   #   high = "yellow",
  #   #   midpoint = 63,
  #   #   space = "Lab"
  #   # ) +
  #   # ggtitle("Chondrichthyes") +
  #   scale_fill_gradientn(
  #     labels = c("1", seq(25, 125, 25)),
  #     breaks = c(1, seq(25, 125, 25)),
  #     guide = guide_colorbar(
  #       title = "No.\nrearrangements\n(Chondrichthyes)",
  #       title.position = "top",
  #       title.vjust = 1,
  #       # label.position = "left"
  #     ),
  #     colours = hcl.colors(5, palette = "spectral", rev = TRUE),
  #     # values = rescale(c(0, 30, 40, 50, 90, 110, 125)),
  #     limits = c(1, 126),
  #     na.value = "white"
  #   ) +
  #   scale_x_discrete(limit = c(rev(header))) +
  #   theme(
  #     axis.title.x = element_blank(),
  #     axis.text.x = element_text(angle = 90, vjust = 0.3),
  #     axis.title.y = element_blank(),
  #     # legend.direction = "horizontal",
  #     # legend.background = element_rect(fill = "transparent"),
  #     # legend.title = element_text(color = "white"),
  #     # legend.text = element_text(color = "white"),
  #     # legend.key.size = unit(0.5, "cm"),
  #     legend.justification = c(1, 1),
  #     legend.position = c(1, 1),
  #     # legend.key.height = unit(1.25, "cm"),
  #     # legend.key.height = unit(6.5, "cm"), # Makes sense in scale=3
  #     # legend.text.align = 1
  #   )
  # 
  # # ggsave(out_file, height=20, width = 20)
  
  
  # superheat(rearrange_mat,
  #           pretty.order.rows = TRUE,
  #           pretty.order.cols = TRUE,
  #           heat.pal = c("red", "white", "blue"),
  #           left.label = 'variable'
  #           )
  
  Heatmap(rearrange_mat,
           # cutree_rows = 5,
           # cutree_cols = 6
  )
} else if (plot_rearrange_Mammalia) {
  in_file = "rearrange_count_Mammalia.tsv"
  out_file = "rearrange_count_Mammalia.pdf"
  rearrange <-
    read.table(in_file, header = TRUE)
  rearrange_mat <- as.matrix(rearrange)
  header <- colnames(rearrange_mat)
  molten_rearrange_mat <- melt(rearrange_mat)

  ggplot() +
    geom_tile(
      molten_rearrange_mat,
      mapping = aes(x = Var2, y = Var1, fill = value),
      colour = "white"
    ) +
    coord_fixed() +
    scale_fill_gradientn(
      labels = c("1", seq(50, 278, 50)),
      breaks = c(1, seq(50, 278, 50)),
      guide = guide_colorbar(
        title = "No.\nrearrangements\n(Mammalia)",
        title.position = "top",
        title.vjust = 1,
      ),
      colours = hcl.colors(5, palette = "spectral", rev = TRUE),
      limits = c(1, 279),
      na.value = "white"
    ) +
    scale_x_discrete(limit = c(rev(header))) +
    theme(
      axis.title.x = element_blank(),
      axis.text.x = element_text(angle = 90, vjust = 0.3),
      axis.title.y = element_blank(),
      legend.justification = c(1, 1),
      legend.position = c(1, 1)
    )
  
  ggsave(out_file, height=49.9, width = 49.9)
} else if (plot_nrc_Chondrichthyes) {
  # ent <- read.table("ent_Chondrichthyes.tsv", header = TRUE)
  # ent_mat <- as.matrix(ent)
  # melted_ent_mat <- melt(ent_mat)
  # header <- colnames(ent_mat)
  # corr <- round(cor(ent), 1)
  # # p.mat <- cor_pmat(ent)
  # 
  # ggcorrplot(corr,
  #            hc.order = TRUE,
  #            type = "upper",
  #            outline.col = "white") +
  #   ggtitle("Chondrichthyes") +
  #   # scale_fill_gradientn(
  #   #   # labels = c("1", seq(25,125,25)),
  #   #   # breaks = c(1, seq(25,125,25)),
  #   #   # guide = guide_colorbar(
  #   #   #   title = "No.\nrearrangements\n(Chondrichthyes)",
  #   #   #   title.position = "top",
  #   #   #   # title.hjust = 0,
  #   #   #   title.vjust = 1,
  #   #   #   # label.position = "left"
  #   #   # ),
  #   #   colours = hcl.colors(5, palette = "spectral", rev = TRUE),
  #   #   # values = rescale(c(0, 30, 40, 50, 90, 110, 125)),
  #   #   # limits = c(1, 126),
  #   #   na.value = "white"
  #   # ) +
  #   scale_x_discrete(position = "top") +
  #   # scale_x_discrete(limit = c(rev(header))) +
  #   # theme_bw() +
  #   theme(
  #     axis.title.x = element_blank(),
  #     axis.title.y = element_blank(),
  #     axis.text.x = element_text(angle = 90, vjust = 0.3),
  #     # legend.direction = "horizontal",
  #     # legend.background = element_rect(fill = "transparent"),
  #     # legend.title = element_text(color = "white"),
  #     # legend.text = element_text(color = "white"),
  #     # legend.key.size = unit(0.5, "cm"),
  #     legend.justification = c(1, 0.5),
  #     legend.position = c(1, 0.5),
  #     # legend.key.height = unit(1.25, "cm"),
  #     # legend.key.height = unit(6.5, "cm"), # Makes sense in scale=3
  #     # legend.text.align = 1
  #   )
# 
#   # corrplot(
#   #   corr,
#   #   type = "upper",
#   #   order = "hclust",
#   #   method = "color",
#   #   tl.col = "black",
#   #   addrect = 5,
#   #   addgrid.col = "white",
#   #   col = hcl.colors(9, palette = "spectral", rev = TRUE)
#   # )
#   
#   # ggsave("mori", width = 20, height = 19)
  
  
  # in_file = "ent_Chondrichthyes.tsv"
  # out_file = "ent_Chondrichthyes.pdf"
  # nrc <- read.table(in_file, header = TRUE)
  # nrc_mat <- as.matrix(nrc)
  # header <- colnames(nrc_mat)
  
  # row.order <- hclust(dist(nrc))$order # clustering
  # col.order <- hclust(dist(t(nrc)))$order
  # # dat_new <- nrc[row.order, col.order] # re-order matrix accoring to clustering
  # # molten_nrc_mat <- melt(as.matrix(dat_new)) # reshape into dataframe
  # 
  # molten_nrc_mat <- melt(nrc_mat)
  # 
  # ggplot() +
  #   geom_tile(
  #     molten_nrc_mat,
  #     mapping = aes(x = Var2, y = Var1, fill = value),
  #     colour = "white"
  #   ) +
  #   coord_fixed() +
  #   scale_fill_gradientn(
  #     limits = c(0, 2),
  #     # labels = c(seq(0, 2, 0.5)),
  #     # breaks = c(seq(0, 2, 0.5)),
  #     guide = guide_colorbar(
  #       title = "Entropy of\nChondrichthyes",
  #       title.position = "top",
  #       title.vjust = 1,
  #     ),
  #     colours = hcl.colors(5, palette = "spectral", rev = FALSE),
  #     na.value = "white"
  #   ) +
  #   scale_x_discrete(limit = c(rev(header))) +
  #   theme(
  #     axis.title.x = element_blank(),
  #     axis.text.x = element_text(angle = 90, vjust = 0.3),
  #     axis.title.y = element_blank(),
  #     legend.key.height = unit(2,"cm"),
  #     # legend.key.width = unit(0.75,"cm"),
  #     # legend.justification = c(1, 1),
  #     # legend.position = c(1, 1)
  #   )

  # ggsave(out_file, width = 20, height = 19)

  
  # row.order <- hclust(dist(nrc_mat))$order
  # col.order <- hclust(dist(t(nrc_mat)))$order
  # row.order <- diana(dist(nrc_mat))$order
  # col.order <- diana(dist(t(nrc_mat)))$order
  # row.order <- agnes(dist(nrc_mat))$order
  # col.order <- agnes(dist(t(nrc_mat)))$order
  # .density = anno_density(nrc_mat, type = "line", gp = gpar(col = "blue"))
  # ha_mix_top = HeatmapAnnotation(density = .density)
  # .violin = anno_density(nrc_mat, type = "violin", gp = gpar(fill = "lightblue"), which = "row")
  # .boxplot = anno_boxplot(nrc_mat, which = "row")
  # ha_mix_right = HeatmapAnnotation(violin = .violin, bxplt = .boxplot, which = "row", width = unit(7, "cm"))
  
  # h <- Heatmap(nrc_mat)
  # h1<-Heatmap(nrc_mat,
  #         name = "NRC",
  #         row_order = row_order(h), column_order = column_order(h),
  #         # row_order = row.order, column_order = col.order,
  #         # top_annotation = ha_mix_top,
  #         # right_annotation = ha_mix_right
  #         )
  # 
  # 
  # in_rearrange_file = "rearrange_count_Chondrichthyes_symmetric.tsv"
  # rearrange_mat <- as.matrix(read.table(in_rearrange_file, header = TRUE))
  # h2<-Heatmap(rearrange_mat,
  #         name = "# rearrange",
  #         row_order = row_order(h), column_order = column_order(h)
  #         )
  # 
  # h1+h2
  
  
  class <- 'Chondrichthyes'
  # class <- 'Mammalia'
  # class <- 'Actinopterygii'
  
  in_file = paste('ent_', class, '.tsv', sep='')
  nrc <- read.table(in_file, header = TRUE)
  nrc_mat <- as.matrix(nrc)

  H <- Heatmap(nrc_mat)
  a <- Heatmap(
    nrc_mat,
    row_order = row_order(H),
    column_order = column_order(H),
    row_names_side = c("left"),
    name = "NRC",
    col = colorRamp2(
      breaks = seq(0, 2, 1),
      colors = c("#A71B4B", "#FEFDBE", "#584B9F")
      # colors = c('red', 'white', 'blue')
    ),
    column_title = paste("Normalized relative compression (NRC) in", class)
    # row_order = row.order, column_order = col.order,
    # top_annotation = ha_mix_top,
    # right_annotation = ha_mix_right
  )

  in_rearrange_file = paste('rearrange_count_', class, '_symmetric.tsv', 
                            sep = '')
  rearrange_mat <-
    as.matrix(read.table(in_rearrange_file, header = TRUE))
  b <- Heatmap(
    rearrange_mat,
    row_order = row_order(H),
    column_order = column_order(H),
    name = "# rearrange",
    col = colorRamp2(
      breaks = seq(0, 50, 10),
      # colors = c('blue', 'white', 'yellow', 'green', 'red')
      colors = c("#584B9F", "#00B1B5", "#BAEEAE", "#FCDE85", "#ED820A", "#A71B4B")
      # colors = hcl.colors(5, palette = "spectral", rev = TRUE)
    ),
    column_title = paste("Number of rearrangements in", class)
  )

  # pdf(paste(class, ".pdf", sep=''), width = 40, height = 20)
  draw(a+b)
  # dev.off()
} else if (plot_nrc_Mammalia) {
  ent <- read.table("ent_Mammalia.tsv", header = TRUE)
  ent_mat <- as.matrix(ent)
  melted_ent_mat <- melt(ent_mat)
  header <- colnames(ent_mat)
  corr <- round(cor(ent), 1)
  # p.mat <- cor_pmat(ent)
  
  ggcorrplot(corr,
             hc.order = TRUE,
             type = "upper",
             outline.col = "white") +
    ggtitle("Mammalia") +
    # scale_fill_gradientn(
    #   # labels = c("1", seq(25,125,25)),
    #   # breaks = c(1, seq(25,125,25)),
    #   # guide = guide_colorbar(
    #   #   title = "No.\nrearrangements\n(Chondrichthyes)",
    #   #   title.position = "top",
    #   #   # title.hjust = 0,
    #   #   title.vjust = 1,
    #   #   # label.position = "left"
    #   # ),
    #   colours = hcl.colors(5, palette = "spectral", rev = TRUE),
  #   # values = rescale(c(0, 30, 40, 50, 90, 110, 125)),
  #   # limits = c(1, 126),
  #   na.value = "white"
  # ) +
  scale_x_discrete(position = "top") +
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
      legend.justification = c(1, 0.5),
      legend.position = c(1, 0.5),
      # legend.key.height = unit(1.25, "cm"),
      # legend.key.height = unit(6.5, "cm"), # Makes sense in scale=3
      # legend.text.align = 1
    )
} else if (cluster_Chondrichthyes) {
  ent <- read.table("ent_Chondrichthyes.tsv", header = TRUE)
  # heatmaply(scale(ent), k_row = 4, k_col = 4)
  
  # df<-data.frame(ent)
  # df <- scale(df)
  # d <- dist(df, method = "euclidean")
  # hc1 <- hclust(d, method = "complete" )
  # hc3 <- agnes(df, method = "ward")
  # # pltree(hc3, cex = 0.6, hang = -1, main = "Dendrogram of agnes") 
  # # plot(hc1, cex = 0.6, hang = -1)
  # hc4 <- diana(df)
  # # pltree(hc4, cex = 0.6, hang = -1, main = "Dendrogram of diana")
  # 
  # hc5 <- hclust(d, method = "ward.D2" )
  # sub_grp <- cutree(hc5, k = 6)
  # table(sub_grp)
  # fviz_cluster(list(data = df, cluster = sub_grp))
  
  
  # ent_mat <- as.matrix(ent)
  # melted_ent_mat <- melt(ent_mat)
  # header <- colnames(ent_mat)
  # corr <- round(cor(ent), 1)
  # # p.mat <- cor_pmat(ent)
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