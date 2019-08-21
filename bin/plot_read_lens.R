#!/usr/bin/env Rscript

args<-commandArgs(TRUE)

if (length(args) < 1) {
  cat("USAGE: plot_read_lens.R cov1 cov2 ... covn")
} else {

  cat("making a table of length", length(args), "\n")

  covs <- vector(mode = "list", length = length(args))

  maxbases = 0
  maxlen   = 0

  for (i in 1:length(args)) {
    cat("loading file[", i, "]: ", args[[i]], "\n", sep="")
    cov = read.table(args[[i]])

    mymaxlen   = max(cov[[1]])
    mymaxbases = max(cov[[2]])
    cat(" total bases:", mymaxbases, "max len:", mymaxlen, "\n")

    covs[[i]] = cov

    if (mymaxbases > maxbases) { maxbases = mymaxbases }
    if (mymaxlen   > maxlen)   { maxlen   = mymaxlen }
  }

  cat("plotting rawcoverage.pdf maxbases:", maxbases, "maxlen:", maxlen, "\n")

  pdf("rawcoverage.pdf")

  plot(mymaxlen, mymaxbases, xlim=c(0,mymaxlen*1.025), ylim=c(0,maxbases*1.025), xlab="min read length", ylab="bases available", main="Raw Coverage")
  rect(-1e7,-mymaxbases*2,1e7,mymaxbases*2, col="lightgrey")

  abline(v=seq(0, maxlen, 10000), col="white", lty=2)
  abline(h=seq(0, maxbases, 1e10), col="white", lty=2)

  for (i in 1:length(args)) {
    cov = covs[[i]]
    lines(cov[[1]], cov[[2]], typ="l", lwd=4, col=i)
  }

  legend("topright", inset=0.02, legend=args, fill=1:(length(args)), bg="white")

  dev.off()

  cat("plotting percentage of bases\n")

  pdf("percentcoverage.pdf")

  plot(mymaxlen, 100, xlim=c(0,mymaxlen*1.025), ylim=c(0,100*1.025), xlab="min read length", ylab="percent bases available", main="Percent Bases")
  rect(-1e7,-mymaxbases*2,1e7,mymaxbases*2, col="lightgrey")

  abline(v=seq(0, maxlen, 10000), col="white", lty=2)
  abline(h=seq(0, 100, 10), col="white", lty=2)

  for (i in 1:length(args)) {
    cov = covs[[i]]
    lines(cov[[1]], cov[[3]]*100, typ="l", lwd=4, col=i)
  }

  legend("topright", inset=0.02, legend=args, fill=1:(length(args)), bg="white")

  dev.off()
}
