#!/usr/bin/env Rscript

args<-commandArgs(TRUE)

if (length(args) < 1) {
  cat("USAGE: plothist.R file.txt [hist_min hist_max hist_breaks hist_span]\n")
} else {
  datafile = args[[1]]
  outfile = paste(datafile, ".hist.pdf", sep="")

  histmin = 0
  histmax = 50000
  histbreaks = 2000
  histspan = 10000

  if (length(args) > 1) { histmin    = as.numeric(args[[2]]) }
  if (length(args) > 2) { histmax    = as.numeric(args[[3]]) }
  if (length(args) > 3) { histbreaks = as.numeric(args[[4]]) }
  if (length(args) > 4) { histspan   = as.numeric(args[[5]]) }

  cat(paste("making a histogram of \"", datafile, "\" in \"", outfile, "\"\n", sep=""))
  cat("hist_min:", histmin, "histmax:", histmax, "histbreaks:", histbreaks, "histspan", histspan, "\n")

  s <- scan(datafile)
  pdf(outfile)
  options(scipen=5)
  a<-hist(s, breaks=histbreaks, xlim=c(histmin,histmax), xlab="read length", ylab="Frequency", main="Read Length Histogram")
  rect(par("usr")[1],par("usr")[3],par("usr")[2],par("usr")[4],col = "gray")
  abline(v=seq(histmin, histmax, histspan), col="white")
  lines(a, col="blue")
  dev.off()
}
