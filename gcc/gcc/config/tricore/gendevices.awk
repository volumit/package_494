# Copyright (C) 2012-2014 Free Software Foundation, Inc.
#
# This file is part of GCC.
#
# GCC is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3, or (at your option) any later
# version.
#
# GCC is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with GCC; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.

##################################################################
#  
# Transform Device / Errata Information from devices.def to a
# Representation that is understood by GCC.
#
# The Script works as a Filter from STDIN to STDOUT.
# 
# FORMAT = "Makefile"    : Generate Makefile Snippet that sets
#                          some MULTILIB_MATCHES  as needed.
# FORMAT = "opt"         : Generate in opt file that spells out
#                          the valid arguments to -mcpu=
#                          and -m[no-]errata=
# FORMAT = "errata.texi" : Generate a snip for doc/extend.texi.
#
# FORMAT = "tricore_errata.tex"
#                        : Generate a snip for the usersguide.
#
##################################################################

BEGIN {
    FS ="[(, \t]+"
    comment = 1
    cpu     = 0
    tric_devices = "TRIC_DEVICES ="

    if (FORMAT == "errata.texi")
    {
	na = "--"
	do_as[0]  = na
	do_as[1]  = "yes"
	do_as[-1] = "yes (sanity check)"

	do_cc[1]  = "yes"
	do_cc[0]  = na
	do_cc[-1] = na " (to be done)"
	do_cc[-2] = na " (not needed)"

	do_core["TRIC_13"]  = "1.3.0"
	do_core["TRIC_131"] = "1.3.1"

	print "@c start errata.texi"
	print ""
	print "@c Auto-generated texi snip to list TriCore errata"
	print "@c Generated by    : config/tricore/gendevices.awk"
	print "@c Generated from  : config/tricore/errata.def"
	print "@c Used by         : -none- can be used by copy-pasting to"
	print "@c                 : doc/invoke.texi"
	print ""
	printf "@multitable {cpuWWW} {x (not needed)} {yes (sanity check)}"
	print " {ERRATA_CPU} {W.W.W}\n"
	printf "@headitem @code{-merrata=} @tab by Compiler @tab by Assembler"
	print "@tab Macro @tab Core"
    }

    if (FORMAT == "tricore_errata.tex")
    {
	na = "--"
	do_as[0]  = na
	do_as[1]  = "yes"
	do_as[-1] = "yes\\footnote{sanity check}"

	do_cc[1]  = "yes"
	do_cc[0]  = na
	do_cc[-1] = na "\\footnote{to be done}"
	do_cc[-2] = na "\\footnote{not needed}"

	do_core["TRIC_13"]  = "1.3"
	do_core["TRIC_131"] = "1.3.1"

	print "% start tricore_errata.tex"
	print ""
	print "% Auto-generated LaTeX snip to list TriCore errata"
	print "% Generated by    : config/tricore/gendevices.awk"
	print "% Generated from  : config/tricore/errata.def"
	print "% Used by         : -none- can be used by copy-pasting to"
	print "%                 : $usersguide/tables/tricore_errata.tex"
	print ""
	print "\\begin{longtable}[hbt!]{|l|l|l|l|l|l|}\\hline\\centering"
	printf "Erratum & \\verb+-merrata=+ & cc1 & as & Built-in Macro & Core "
	print "\\\\ \\hline"
    }
}

function trim (str) {
    gsub ("\"", "", str)
    gsub (/\)/,  "", str)
    return str
}


##################################################################
# Add some Comments to the generated Files and copy-paste
# Copyright Notice from above.
##################################################################

/^#/ {
    if (!comment)
	next
    else if (comment == 1)
    {
	if (FORMAT == "Makefile")
	{
	    print "# Auto-generated Makefile Snip"
	    print "# Generated by    : config/tricore/gendevices.awk"
	    print "# Generated from  : config/tricore/devices.def"
	    print "# Used by         : config.gcc (tmake_file)"
	    print "#                 : config/tricore/t-tricore"
	    print ""
	}

	if (FORMAT == "opt")
	{
	    print ";; Auto-generated Option File"
	    print ";; Generated by    : config/tricore/gendevices.awk"
	    print ";; Generated from  : config/tricore/devices.def"
	    print ";;                 : config/tricore/errata.def"
	    print ";; Used by         : config.gcc (extra_options)"
	    print ""
	}
    }

    comment = 2;

    if (FORMAT == "opt")
    {
	gsub ("#", ";", $0)
    }

    print
}

/^$/ {
    # The first empty line stops copy-pasting the GPL comments
    # from this file to the generated file.

    if (comment == 2)
    {
	if (FORMAT == "Makefile")
	{
	    print
	    printf "MULTILIB_MATCHES = "
	}

	if (FORMAT == "opt")
	{
	    print

	    print "Enum"
	    print "Name(tric_mcpu) Type(int)"
	    print "Known CPU names:\n"

	    print "Enum"
	    print "Name(tric_merrata) Type(int)"
	    print "Known silicon BUG names to work around:\n"

	    print "Enum"
	    print "Name(tric_mnoerrata) Type(int)"
	    print " \n" # Undocumented
	}
    }

    comment = 0
}

##################################################################
# Run over all DEF_DEVICE Lines and gather Information:
##################################################################

/^DEF_DEVICE/ {
    name = trim($2)
    core = trim($4)

    if (FORMAT == "Makefile")
    {
	printf "\\\n\tmtc" core "=mcpu?" name " "
	tric_devices = tric_devices " " name
    }
    
    if (FORMAT == "opt")
    {
	print  "EnumValue"
	printf "Enum(tric_mcpu) String(%s) Value(%d)\n\n", name, cpu++
    }

    next
}

/^DEF_ERRATA/ {
    bug = trim($2)

    if (FORMAT == "opt")
    {
	print  "EnumValue"
	printf "Enum(tric_merrata) String(%s) Value(tric_errata_%s)\n\n",
	    bug, bug

	print  "EnumValue"
	printf "Enum(tric_mnoerrata) String(%s) Value(tric_errata_%s)\n\n",
	    bug, bug
    }

    if (FORMAT == "errata.texi")
    {
	bug = "@code{" bug "}"
	by_cc = do_cc[trim($3)]
	by_as = do_as[trim($4)]
	macro = "@code{ERRATA_" trim($5) "}"
	by_core  = do_core[trim($6)]
	ifx_name = trim($7)
	_ = " @tab "

	print "@item " bug _ by_cc _ by_as _ macro _ by_core
    }

    if (FORMAT == "tricore_errata.tex")
    {
	bug = "\\verb+" bug "+"
	by_cc = do_cc[trim($3)]
	by_as = do_as[trim($4)]
	macro = "\\verb+ERRATA_" trim($5) "+"
	by_core  = do_core[trim($6)]
	ifx_name = trim($7)
	gsub ("_",  "\\_", ifx_name)

	_ = " & "

	print ifx_name _ bug _ by_cc _ by_as _ macro _ by_core "\\\\ \\hline"
    }

    next
}

END {
    print ""

    if (FORMAT == "Makefile")
	printf "\n" tric_devices "\n"

    if (FORMAT == "errata.texi")
    {
	print "@end multitable"
	print ""
	print "@c end errata.texi"
    }

    if (FORMAT == "tricore_errata.tex")
    {
	print "\\caption{Errata}"
	print "\\label{tab:erratas}"
	print "\\end{longtable}"
	print ""
	print "% end tricore-errata.tex"
    }
}
