# rearrange <- read.table('rearrange_count_Chondrichthyes.tsv', sep = '\t', header = TRUE)
# rearrange <- read.table('rearrange_count_Mammalia.tsv', sep = '\t', header = TRUE)
# rearrange <- read.table('rearrange_count_Actinopterygii.tsv', sep = '\t', header = TRUE)
# molten.rearrange <- melt(rearrange)

names <- read.table('COX1_pos.tsv', sep='\t', header = TRUE)

print(names$Organism[names$Name=='NC_012737'])