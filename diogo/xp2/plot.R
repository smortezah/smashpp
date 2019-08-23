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

plot_nrc_rearrange_Chondrichthyes <- FALSE
plot_nrc_rearrange_Mammalia <- FALSE
plot_nrc_rearrange_Actinopterygii <- T

plot.nrc.rearrange <- function(class, width, height,
                               rearange_breaks, rearrange_colors) {
  nrc_file_name <- paste('ent_', class, '.tsv', sep = '')
  nrc_mat <- as.matrix(read.table(nrc_file_name, header = TRUE))
  
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
    ),
    column_title = paste("Normalized relative compression (NRC) in", class)
  )
  
  rearrange_file_name <- 
    paste('rearrange_count_', class, '_symmetric.tsv', sep = '')
  rearrange_mat <- as.matrix(read.table(rearrange_file_name, header = TRUE))
  
  if (missing(rearange_breaks) | missing(rearrange_colors)){
    b <- Heatmap(
      rearrange_mat,
      row_order = row_order(H),
      column_order = column_order(H),
      name = "# rearrange",
      column_title = paste("Number of rearrangements in", class)
    )
  } else {
    b <- Heatmap(
      rearrange_mat,
      row_order = row_order(H),
      column_order = column_order(H),
      name = "# rearrange",
      col = colorRamp2(
        breaks = rearange_breaks,
        colors = rearrange_colors
      ),
      heatmap_legend_param = list(
        at = rearange_breaks,
        labels = rearange_breaks
      ),
      column_title = paste("Number of rearrangements in", class)
    )
  }
  
  pdf(paste(class, ".pdf", sep = ''), width = width, height = height)
  draw(a + b)
  dev.off()
}

if (plot_nrc_rearrange_Chondrichthyes) {
  plot.nrc.rearrange('Chondrichthyes',
                     40,
                     20,
                     seq(0, 45, 15),
                     hcl.colors(4, palette = "spectral", rev = TRUE))
} else if (plot_nrc_rearrange_Mammalia) {
  plot.nrc.rearrange('Mammalia',
                     200,
                     100,
                     seq(0, 60, 20),
                     hcl.colors(4, palette = "spectral", rev = TRUE))
} else if (plot_nrc_rearrange_Actinopterygii) {
  plot.nrc.rearrange('Actinopterygii',
                     200,
                     100,
                     seq(0, 45, 15),
                     hcl.colors(4, palette = "spectral", rev = TRUE))
}