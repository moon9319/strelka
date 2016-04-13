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
/// \brief misc utilities to summarize and/or transform alignment objects
///

#pragma once


#include "blt_util/pos_range.hh"
#include "htsapi/bam_record.hh"
#include "starling_common/alignment.hh"
#include "starling_common/indel.hh"


/// Provides conservative ref coordinate bounds of an alignment
///
/// leading/trailing insertions and soft-clip will
/// not be counted if using this range
///
known_pos_range
get_strict_alignment_range(const alignment& al);

/// Provides ref coordinate bounds of an alignment when edge insertions
/// are converted to match, but soft-clip segments are left as-is
///
known_pos_range
get_soft_clip_alignment_range(const alignment& al);

/// Provides ref coordinate bounds of an alignment when edge insertions
/// and soft clip are converted to match.
///
/// For instanace pos=10 CIGAR=2S2M2S should return [8,15)
///
known_pos_range
get_alignment_range(const alignment& al);


/// Provides the largest reasonable ref coordinate bounds of an alignment by
/// starting from get_alignment_range() output, and requiring that
/// the end of the alignment range is equal to at least
/// range_start+seq_length and the start of the alignment range is
/// equal to at least range_end-seq_length:
///
known_pos_range
get_alignment_zone(const alignment& al,
                   const unsigned seq_length);

/// Return true if indel, ik, is in alignment, al, or is consistent
/// with an edge-insertion in al
///
/// read_indel_pr is set to the left and right breakpoints in the
/// coordinates of al. The left or right value will not be set if the
/// breakpoint does not occur within the read.
///
bool
is_indel_in_alignment(const alignment& al,
                      const indel_key& ik,
                      pos_range& read_indel_pr);

/// remove any edge deletions and properly adjust pos for leading deletions:
alignment
remove_edge_deletions(
    const alignment& al,
    const bool is_remove_leading_edge,
    const bool is_remove_trailing_edge);

/// transform an alignment such that any insert edge segments become
/// match. insertions can be enclosed with soft-clip/hard-clip and will still be
/// counted as edge insertions.
///
/// segments are joined and start pos is adjusted appropriately
///
alignment
matchify_edge_insertions(
    const alignment& al,
    const bool is_match_leading_edge,
    const bool is_match_trailing_edge);

/// replicates behavior of matchify_edge_insertions() with an additional
/// step to remove edge deletions
///
alignment
matchify_edge_indels(
    const alignment& al,
    const bool is_match_leading_edge,
    const bool is_match_trailing_edge);

/// transform an alignment such that any soft-clipped edge segments
/// become match.
///
/// segments are joined and start pos is adjusted appropriately
///
alignment
matchify_edge_soft_clip(
    const alignment& al);

/// translate bam_record into the cooresponding alignment structure
///
void
getAlignmentFromBamRecord(
    const bam_record& br,
    alignment& al);

#if 0
/// Shift all indels as far "to the left" as possible -- note that
/// some indels may be lost. Returns true if the alignment was changed.
///
bool
normalize_alignment(alignment& al,
                    const std::string& read_seq,
                    const std::string& ref_seq);
#endif

