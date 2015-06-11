#include "boost/test/unit_test.hpp"

#include "starling_shared.hh"

#include "blt_util/reference_contig_segment.cpp"
#include "blt_util/seq_util.hh"
#include "blt_common/snp_pos_info.cpp"
#include "starling_common/pos_basecall_buffer.cpp"
#include "starling_common/starling_base_shared.hh"
#include "codon_phaser.cpp"
#include "gvcf_locus_info.cpp"

static void insert_read(const char* read, pos_t position,
        pos_basecall_buffer& bc_buff)
{
    pos_t insert_pos = position;
    while (*read)
    {
        base_call bc(base_to_id(*read), 30, true, 0, 0, false, false, false,
                insert_pos == position, !*(read + 1));
        bc_buff.insert_pos_basecall(insert_pos, true, bc);
        insert_pos++;
        read++;
    }

}



BOOST_AUTO_TEST_SUITE( codon_phaser )

class dummy_variant_sink : public variant_pipe_stage_base
{
public:
    dummy_variant_sink() : variant_pipe_stage_base() {}
    std::vector<site_info> the_sites;
    std::vector<indel_info> the_indels;
    void process(site_info& si) override
    {
        if (si.is_het() || si.is_hetalt() ) the_sites.push_back(si);
    }
    void process(indel_info& ii) override { the_indels.push_back(ii); }
};

// positive tests

BOOST_AUTO_TEST_CASE( simple_3mer )
{
    std::cout << "*****************************************" << std::endl;
    reference_contig_segment rcs;
    rcs.seq() = "ACGTACGTACGT";
    pos_basecall_buffer bc_buff(rcs);

    auto r1 = "ACGTACGTAC";
    auto r2 = "ACGTGCTTAC";
    pos_t read_pos = 0;

    // add 2 haplotypes of reads
    for (int i = 0; i < 10; i++)
        insert_read(r1, read_pos, bc_buff);
    for (int i = 0; i < 10; i++)
        insert_read(r2, read_pos, bc_buff);

    starling_base_options opt;
    opt.phasing_window = 3;
    opt.do_codon_phasing = true;

    dummy_variant_sink sink;
    Codon_phaser phaser(opt, bc_buff, rcs, sink);

    for (int i = 0; r1[i]; i++)
    {
        site_info si;
        const snp_pos_info& spi(bc_buff.get_pos(read_pos + i));
        si.init(read_pos + i, rcs.get_base(read_pos + i), spi, 30);
        si.smod.is_covered = si.smod.is_used_covered = true;
        si.dgt.ref_gt = base_to_id(si.ref);

        si.smod.max_gt = DIGT::get_gt_with_alleles(base_to_id(r1[i]),base_to_id(r2[i]));
        si.dgt.is_snp = si.ref != r1[i] || si.ref != r2[i];

        phaser.process(si);
    }
    phaser.flush();
    BOOST_CHECK_EQUAL("GCT", sink.the_sites.front().phased_alt);

}

BOOST_AUTO_TEST_CASE( two_adjacent_3mers )
{
    reference_contig_segment rcs;
    rcs.seq() = "ACGTACGTACGTACGT";
    pos_basecall_buffer bc_buff(rcs);

    auto r1 = "ACGTACGTACGTACGT";
    auto r2 = "ACGTGCTTGCTTACGT";
    pos_t read_pos = 0;

    // add 2 haplotypes of reads
    for (int i = 0; i < 10; i++)
        insert_read(r1, read_pos, bc_buff);
    for (int i = 0; i < 10; i++)
        insert_read(r2, read_pos, bc_buff);

    starling_base_options opt;
    opt.phasing_window = 3;
    opt.do_codon_phasing = true;

    dummy_variant_sink sink;
    Codon_phaser phaser(opt, bc_buff, rcs, sink);


    for (int i = 0; r1[i]; i++)
    {
        site_info si;
        const snp_pos_info& spi(bc_buff.get_pos(read_pos + i));
        si.init(read_pos + i, rcs.get_base(read_pos + i), spi, 30);
        si.smod.is_covered = si.smod.is_used_covered = true;
        si.dgt.ref_gt = base_to_id(si.ref);

        si.smod.max_gt = DIGT::get_gt_with_alleles(base_to_id(r1[i]),base_to_id(r2[i]));
        si.dgt.is_snp = si.ref != r1[i] || si.ref != r2[i];

        phaser.process(si);
    }
    phaser.flush();
    BOOST_CHECK_EQUAL("GCTTGCT", sink.the_sites.front().phased_alt);
}

BOOST_AUTO_TEST_CASE( handles_snps_at_start )
{
    reference_contig_segment rcs;
    rcs.seq() = "ACGTACGTACGT";
    pos_basecall_buffer bc_buff(rcs);

    auto r1 = "ACGTACGTAC";
    auto r2 = "GGGTACGTAC";
    pos_t read_pos = 0;

    // add 2 haplotypes of reads
    for (int i = 0; i < 10; i++)
        insert_read(r1, read_pos, bc_buff);
    for (int i = 0; i < 10; i++)
        insert_read(r2, read_pos, bc_buff);

    starling_base_options opt;
    opt.phasing_window = 3;
    opt.do_codon_phasing = true;

    dummy_variant_sink sink;
    Codon_phaser phaser(opt, bc_buff, rcs, sink);

        for (int i = 0; r1[i]; i++)
    {
        site_info si;
        const snp_pos_info& spi(bc_buff.get_pos(read_pos + i));
        si.init(read_pos + i, rcs.get_base(read_pos + i), spi, 30);
        si.smod.is_covered = si.smod.is_used_covered = true;
        si.dgt.ref_gt = base_to_id(si.ref);

        si.smod.max_gt = DIGT::get_gt_with_alleles(base_to_id(r1[i]),base_to_id(r2[i]));
        si.dgt.is_snp = si.ref != r1[i] || si.ref != r2[i];

        phaser.process(si);
    }
    phaser.flush();
    BOOST_CHECK_EQUAL("GG", sink.the_sites.front().phased_alt);
}


BOOST_AUTO_TEST_CASE( respects_phasing_window )
{
    reference_contig_segment rcs;
    rcs.seq() = "ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT";
    pos_basecall_buffer bc_buff(rcs);

    auto r1 = "ACGTACGTACGTACGTACGTACGT";
    auto r2 = "ACGTAAGAACGGAGGTACGTACGT";
    pos_t read_pos = 0;

    // add 2 haplotypes of reads
    for (int i = 0; i < 10; i++)
        insert_read(r1, read_pos, bc_buff);
    for (int i = 0; i < 10; i++)
        insert_read(r2, read_pos, bc_buff);

    starling_base_options opt;
    opt.phasing_window = 5;
    opt.do_codon_phasing = true;

    dummy_variant_sink sink;
    Codon_phaser phaser(opt, bc_buff, rcs, sink);

    for (int i = 0; r1[i]; i++)
    {
        site_info si;
        const snp_pos_info& spi(bc_buff.get_pos(read_pos + i));
        si.init(read_pos + i, rcs.get_base(read_pos + i), spi, 30);
        si.smod.is_covered = si.smod.is_used_covered = true;
        si.dgt.ref_gt = base_to_id(si.ref);

        si.smod.max_gt = DIGT::get_gt_with_alleles(base_to_id(r1[i]),base_to_id(r2[i]));
        si.dgt.is_snp = si.ref != r1[i] || si.ref != r2[i];

        phaser.process(si);
    }
    phaser.flush();
    BOOST_CHECK_EQUAL ("AGAACGGAG", sink.the_sites.front().phased_alt);
}

BOOST_AUTO_TEST_CASE(test_overlapping_phased_snps)
{
    reference_contig_segment rcs;
    rcs.seq() = "ACGTACGTACGT";
    pos_basecall_buffer bc_buff(rcs);

    auto r1 = "ACATGCGTAC";
    auto r2 = "ACCTCCGTAC";
    pos_t read_pos = 0;

    // add 2 haplotypes of reads
    for (int i = 0; i < 10; i++)
        insert_read(r1, read_pos, bc_buff);
    for (int i = 0; i < 10; i++)
        insert_read(r2, read_pos, bc_buff);

    starling_base_options opt;
    opt.phasing_window = 3;
    opt.do_codon_phasing = true;

    dummy_variant_sink sink;
    Codon_phaser phaser(opt, bc_buff, rcs, sink);


    for (int i = 0; r1[i]; i++)
    {
        site_info si;
        const snp_pos_info& spi(bc_buff.get_pos(read_pos + i));
        si.init(read_pos + i, rcs.get_base(read_pos + i), spi, 30);
        si.smod.is_covered = si.smod.is_used_covered = true;
        si.dgt.ref_gt = base_to_id(si.ref);

        si.smod.max_gt = DIGT::get_gt_with_alleles(base_to_id(r1[i]),base_to_id(r2[i]));
        si.dgt.is_snp = si.ref != r1[i] || si.ref != r2[i];

        phaser.process(si);
    }
    phaser.flush();
    BOOST_CHECK_EQUAL("ATG,CTC", sink.the_sites.front().phased_alt);
}

BOOST_AUTO_TEST_CASE(test_overlapping_phased_snps_different_size)
{
    reference_contig_segment rcs;
    rcs.seq() = "ACGTACGTACGT";
    pos_basecall_buffer bc_buff(rcs);

    auto r1 = "ACGAAAGTAC";
    auto r2 = "ACATCCGTAC";
    pos_t read_pos = 0;

    // add 2 haplotypes of reads
    for (int i = 0; i < 10; i++)
        insert_read(r1, read_pos, bc_buff);
    for (int i = 0; i < 10; i++)
        insert_read(r2, read_pos, bc_buff);

    starling_base_options opt;
    opt.phasing_window = 5;
    opt.do_codon_phasing = true;

    dummy_variant_sink sink;
    Codon_phaser phaser(opt, bc_buff, rcs, sink);

    for (int i = 0; r1[i]; i++)
    {
        site_info si;
        const snp_pos_info& spi(bc_buff.get_pos(read_pos + i));
        si.init(read_pos + i, rcs.get_base(read_pos + i), spi, 30);
        si.smod.is_covered = si.smod.is_used_covered = true;
        si.dgt.ref_gt = base_to_id(si.ref);

        si.smod.max_gt = DIGT::get_gt_with_alleles(base_to_id(r1[i]),base_to_id(r2[i]));
        si.dgt.is_snp = si.ref != r1[i] || si.ref != r2[i];

        phaser.process(si);
    }
    phaser.flush();
    BOOST_CHECK_EQUAL("ATCC,GAAA", sink.the_sites.front().phased_alt);
}


// negative tests
BOOST_AUTO_TEST_CASE( just_one_snp )
{
    reference_contig_segment rcs;
    rcs.seq() = "ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT";
    pos_basecall_buffer bc_buff(rcs);

    auto r1 = "ACGTACGT";
    auto r2 = "ACGGACGT";
    pos_t read_pos = 0;

    // add 2 haplotypes of reads
    for (int i = 0; i < 10; i++)
        insert_read(r1, read_pos, bc_buff);
    for (int i = 0; i < 10; i++)
        insert_read(r2, read_pos, bc_buff);

    starling_base_options opt;
    opt.phasing_window = 3;
    opt.do_codon_phasing = true;

    dummy_variant_sink sink;
    Codon_phaser phaser(opt, bc_buff, rcs, sink);

    for (int i = 0; r1[i]; i++)
    {
        site_info si;
        const snp_pos_info& spi(bc_buff.get_pos(read_pos + i));
        si.init(read_pos + i, rcs.get_base(read_pos + i), spi, 30);
        si.smod.is_covered = si.smod.is_used_covered = true;
        si.smod.gq = si.dgt.genome.snp_qphred = si.smod.Qscore = 40;
        si.dgt.ref_gt = base_to_id(si.ref);

        si.smod.max_gt = DIGT::get_gt_with_alleles(base_to_id(r1[i]),base_to_id(r2[i]));
        si.dgt.is_snp = si.ref != r1[i] || si.ref != r2[i];

        phaser.process(si);
    }
    phaser.flush();
    for (auto phased_variant : sink.the_sites)
    {
        BOOST_CHECK(!phased_variant.smod.filters.any());
        BOOST_CHECK(!phased_variant.smod.is_phased_region);
    }
    BOOST_CHECK_EQUAL(sink.the_sites.size(), 1);
}

BOOST_AUTO_TEST_CASE( read_break_causes_phasing_conflict )
{
    reference_contig_segment rcs;
    rcs.seq() = "ACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGTACGT";
    pos_basecall_buffer bc_buff(rcs);

    auto r1 = "ACGTACGTACGTACGT";
    auto r1_nosnp1 = "ACGTACGT";
    auto r1_nosnp2 = "ACGTACGT";
    auto r2 = "ACGTACGGAGGTACGT";

    pos_t read_pos = 0;
    pos_t read_pos2 = strlen(r1) + read_pos;
    pos_t read_pos_snp = read_pos;

    // add 2 haplotypes of reads
    for (int i = 0; i < 10; i++)
    {
        insert_read(r1_nosnp1, read_pos, bc_buff);
        insert_read(r1_nosnp2, read_pos2, bc_buff);
    }

    for (int i = 0; i < 10; i++)
    {
        insert_read(r2, read_pos_snp, bc_buff);
    }

    starling_base_options opt;
    opt.phasing_window = 3;
    opt.do_codon_phasing = true;

    dummy_variant_sink sink;
    Codon_phaser phaser(opt, bc_buff, rcs, sink);


    for (int i = 0; r1[i]; i++)
    {
        site_info si;
        const snp_pos_info& spi(bc_buff.get_pos(read_pos + i));
        si.init(read_pos + i, rcs.get_base(read_pos + i), spi, 30);
        si.smod.is_covered = si.smod.is_used_covered = true;
        si.smod.gq = si.dgt.genome.snp_qphred = si.smod.Qscore = 40;
        si.dgt.ref_gt = base_to_id(si.ref);

        si.smod.max_gt = DIGT::get_gt_with_alleles(base_to_id(r1[i]),base_to_id(r2[i]));
        si.dgt.is_snp = si.ref != r1[i] || si.ref != r2[i];

        phaser.process(si);
    }
    phaser.flush();
    for (site_info& site : sink.the_sites)
    {
        BOOST_CHECK(! site.is_het() || site.smod.filters.test(VCF_FILTERS::PhasingConflict));
        BOOST_CHECK(!site.smod.is_phased_region);
    }
}


// TODO: write tests for:
// allele imbalanced phasing
// nonsense phasing (i.e. the called SNPs are not in the top most common alleles)

BOOST_AUTO_TEST_SUITE_END()

