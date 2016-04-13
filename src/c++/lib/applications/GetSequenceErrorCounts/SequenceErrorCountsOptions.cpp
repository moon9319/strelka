// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Strelka - Small Variant Caller
// Copyright (c) 2009-2016 Illumina, Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//

///
/// \author Chris Saunders
///

#include "blt_util/blt_exception.hh"

#include <sstream>
#include "SequenceErrorCountsOptions.hh"



SequenceErrorCountsDerivOptions::
SequenceErrorCountsDerivOptions(
    const SequenceErrorCountsOptions& opt,
    const reference_contig_segment& ref)
    : base_t(opt,ref)
{
    if (opt.is_depth_filter())
    {
        parse_chrom_depth(opt.chrom_depth_file, chrom_depth);

        //TODO, verify that chroms match bam chroms
        const std::string& chrom_name(opt.bam_seq_name);
        cdmap_t::const_iterator cdi(chrom_depth.find(std::string(chrom_name)));
        if (cdi == chrom_depth.end())
        {
            std::ostringstream oss;
            oss << "ERROR: Can't find chromosome: '" << chrom_name << "' in chrom depth file: " << opt.chrom_depth_file << "\n";
            throw blt_exception(oss.str().c_str());
        }
        max_depth=(cdi->second*opt.max_depth_factor);
        norm_depth=(cdi->second);
        assert(max_depth>=0.);
        assert(norm_depth>=0.);
    }
}

