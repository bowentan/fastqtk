AM_LDFLAGS =
AM_CPPFLAGS = -g \
			  -I$(abs_top_builddir) \
			  -I$(abs_top_srcdir)/include \
			  -I$(abs_top_srcdir)/lib/program_options/include \
			  -I$(abs_top_srcdir)/lib/fastq/include

bin_PROGRAMS = fastqtk
fastqtk_SOURCES = stat.c bcdist.c filter.c fastqtk.c
fastqtk_LDFLAGS = -Wl,-rpath=$(libdir),-rpath=$(abs_top_builddir)/lib/program_options/src/.libs,-L$(abs_top_builddir)/lib/program_options/src/.libs \
				  -Wl,-rpath=$(libdir),-rpath=$(abs_top_builddir)/lib/fastq/src/.libs,-L$(abs_top_builddir)/lib/fastq/src/.libs
fastqtk_LDADD = -lprogram_options -lfastq
