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
#include "blt_util/log.hh"
#include "common/Exceptions.hh"
#include "starling_common/IndelData.hh"

#include <iostream>
#include <sstream>

//#define DEBUG_ID


void
ReadPathScores::
insertAlt(
    const IndelKey& ik,
    const score_t a)
{
    const unsigned ais(static_cast<unsigned>(alt_indel.size()));
    if (ais < 2)
    {
        alt_indel.push_back(std::make_pair(ik,a));
    }
    else
    {
        unsigned min_index(ais);
        score_t min(a);
        for (unsigned i(0); i<ais; ++i)
        {
            if (alt_indel[i].second < min)
            {
                min = alt_indel[i].second;
                min_index = i;
            }
        }
        if (min_index<ais)
        {
            alt_indel[min_index] = std::make_pair(ik,a);
        }
    }
//    log_os << ik << "\n";
}


void
IndelSampleData::
addIndelObservation(
    const IndelObservationData& obs_data)
{
    const bool isAbstractObservation(obs_data.is_external_candidate || obs_data.is_forced_output);

    if (! isAbstractObservation)
    {
        using namespace INDEL_ALIGN_TYPE;

        evidence_t* insertTarget(nullptr);

        if (obs_data.is_noise)
        {
            insertTarget=&noise_read_ids;
        }
        else if (obs_data.iat == GENOME_TIER1_READ)
        {
            insertTarget=&tier1_map_read_ids;
        }
        else if (obs_data.iat == GENOME_TIER2_READ)
        {
            insertTarget=&tier2_map_read_ids;
        }
        else if (obs_data.iat == GENOME_SUBMAP_READ)
        {
            insertTarget=&submap_read_ids;
        }
        else
        {
            assert(false && "Unknown indel alignment type");
        }

        assert (insertTarget != nullptr);
        const auto retval = insertTarget->insert(obs_data.id);
        assert (retval.second);
    }
}



void
IndelData::
addIndelObservation(
    const unsigned sampleId,
    const IndelObservationData& obs_data)
{
#ifdef DEBUG_ID
    log_os << "KATTER: adding obs for indel: " << _ik;
    log_os << "KATTER: obs: " << obs_data;
#endif

    // never reset the flags to false if they are true already
    if (! is_external_candidate) is_external_candidate=obs_data.is_external_candidate;
    if (! is_forced_output) is_forced_output=obs_data.is_forced_output;

    if (! obs_data.insert_seq.empty())
    {
        if((not _ik.is_breakpoint()) and (obs_data.insert_seq.size() != _ik.insert_length()))
        {
            using namespace illumina::common;

            std::ostringstream oss;
            oss << "ERROR: indel obs insert length '" << obs_data.insert_seq.size() << "' does not match expected length for indel: " << _ik;
            BOOST_THROW_EXCEPTION(LogicException(oss.str()));
        }
        _insert_seq.addObservation(obs_data.insert_seq);
    }

    getSampleData(sampleId).addIndelObservation(obs_data);
}



std::ostream&
operator<<(
    std::ostream& os,
    const IndelObservationData& obs)
{
    os << "is_noise: " << obs.is_noise << "\n";
    os << "is_external: " << obs.is_external_candidate << "\n";
    os << "is_forced_output: " << obs.is_forced_output << "\n";
    os << "type: " << INDEL_ALIGN_TYPE::label(obs.iat) << "\n";
    os << "align_id: " << obs.id << "\n";
    os << "insert_seq: " << obs.insert_seq << "\n";
    return os;
}



static
void
report_indel_evidence_set(
    const IndelSampleData::evidence_t& e,
    const char* label,
    std::ostream& os)
{
    typedef IndelSampleData::evidence_t::const_iterator viter;
    viter i(e.begin()),i_end(e.end());
    for (unsigned n(0); i!=i_end; ++i)
    {
        os << label << " no: " << ++n << " id: " << *i << "\n";
    }
}





std::ostream&
operator<<(
    std::ostream& os,
    const ReadPathScores& rps)
{
    os << "ref: " << rps.ref
       << " indel: " << rps.indel
       << " nsite: " << rps.nsite;

#if 0
    if (rps.is_alt)
    {
        os << " alt: " << rps.alt;
    }
#else
    for (const auto& indel : rps.alt_indel)
    {
        const IndelKey& ik(indel.first);
        os << " alt-" << ik.pos << "-" << INDEL::get_index_label(ik.type) << ik.length << ": " << indel.second;
    }
#endif

    os << " ist1?: " << rps.is_tier1_read;

    return os;
}



void
InsertSequenceManager::
_exception(const char* msg) const
{
    std::ostringstream oss;
    oss << "Exception in InsertSequenceManager: " << msg;
    throw blt_exception(oss.str().c_str());
}



void
InsertSequenceManager::
_finalize()
{
    unsigned count(0);
    std::string& candidate(_consensus_seq);

    for (const auto& val : _obs)
    {
        if (val.first.size() < candidate.size()) continue;
        if (val.first.size() == candidate.size())
        {
            if (val.second <= count) continue;
        }
        candidate = val.first;
        count = val.second;
    }
    _consensus_seq = candidate;
    _obs.clear();
    _is_consensus=true;
}



std::ostream&
operator<<(
    std::ostream& os,
    const IndelSampleData& indelSampleData)
{
    report_indel_evidence_set(indelSampleData.tier1_map_read_ids,"tier1_map_read",os);
    report_indel_evidence_set(indelSampleData.tier2_map_read_ids,"tier2_map_read",os);
    report_indel_evidence_set(indelSampleData.submap_read_ids,"submap_read",os);
    report_indel_evidence_set(indelSampleData.noise_read_ids,"noise_read",os);

    {
        unsigned n(0);
        for (const auto& readLnp : indelSampleData.read_path_lnp)
        {
            os << "read_path_lnp no: " << ++n
               << " id: " << readLnp.first
               << " " << readLnp.second
               << "\n";
        }
    }

    report_indel_evidence_set(indelSampleData.suboverlap_tier1_read_ids,"suboverlap_tier1_read",os);
    report_indel_evidence_set(indelSampleData.suboverlap_tier2_read_ids,"suboverlap_tier2_read",os);

    return os;
}



std::ostream&
operator<<(
    std::ostream& os,
    const IndelData& indelData)
{
    os << "IndelKey: " << indelData._ik << "\n";
    os << "is_external_candidate: " << indelData.is_external_candidate << "\n";
    os << "is_forced_output: " << indelData.is_forced_output << "\n";

    os << "seq: " << indelData.getInsertSeq() << "\n";

    const unsigned sampleCount(indelData.getSampleCount());
    for (unsigned sampleIndex(0); sampleIndex<sampleCount; ++sampleIndex)
    {
        os << "BEGIN sample: " << sampleIndex << "\n";
        os << indelData.getSampleData(sampleIndex);
        os << "BEGIN sample: " << sampleIndex << "\n";
    }
    return os;
}