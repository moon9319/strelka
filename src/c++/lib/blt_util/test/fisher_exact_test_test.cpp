// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Starka
// Copyright (c) 2009-2014 Illumina, Inc.
//
// This software is provided under the terms and conditions of the
// Illumina Open Source Software License 1.
//
// You should have received a copy of the Illumina Open Source
// Software License 1 along with this program. If not, see
// <https://github.com/sequencing/licenses/>
//

#include "boost/test/unit_test.hpp"

#include "blt_util/fisher_exact_test.hh"


template <class T, size_t N>
static size_t carray_size(T (&)[N]) { return N; }

BOOST_AUTO_TEST_SUITE( test_fisher_exact_test )

BOOST_AUTO_TEST_CASE( test_fisher_exact_pval )
    {
        // wikipedia example
        // https://en.wikipedia.org/wiki/Fisher%27s_exact_test
        double test_pval = fisher_exact_test_pval_2x2(0, 10, 12, 2);
        BOOST_CHECK_CLOSE(6.73038093956118699e-05, test_pval, 0.00000001);
    }

BOOST_AUTO_TEST_CASE( test_fisher_exact_examples )
    {
// generate test data in R:
//            library(plyr)
//
//            N = 100
//            testdata = ldply(round(200*runif(N)), function(N) {
//                    r = round(N*runif(1))
//                    n = round(N*runif(1))
//                    urn = c(rep(1, r), rep(0, N-r))
//            # make it slightly more likely we draw defective ones
//                    prob= c(rep(1.2, r), rep(1, N-r))
//                    prob = prob / sum(prob)
//                    a = sum(sample(urn, n, replace=F, prob=prob))
//                    b = n - a   ## sampled non-defective ones
//                    c = r - a   ## remaining defective
//                    d = N - a - b - c ## remaining good ones
//                    fisher_p_t = fisher.test(rbind(c(a, b), c(c, d)))$p.value
//                    fisher_p_l = fisher.test(rbind(c(a, b), c(c, d)), alternative="less")$p.value
//                    fisher_p_g = fisher.test(rbind(c(a, b), c(c, d)), alternative="greater")$p.value
//                    return(data.frame(N=n, r=r, n=n, a=a, b=b, c=c, d=d, fisher_p_t=fisher_p_t, fisher_p_l=fisher_p_l, fisher_p_g=fisher_p_g))
//            })
//            testdata[, c("a", "b", "c", "d", "fisher_p_t", "fisher_p_l", "fisher_p_g")]
        const double example_data[126][7] = {
                {0,   0,   4,   122, 1.0000000,    1.0000000,   1.0000000},
                {1,   1,   110, 49,  0.5260093,    0.5260093,   0.9048913},
                {1,   3,   45,  133, 1.0000000,    0.7347866,   0.6917141},
                {0,   5,   4,   95,  1.0000000,    0.8186761,   1.0000000},
                {5,   10,  24,  53,  1.0000000,    0.6870069,   0.5446537},
                {14,  1,   47,  5,   1.0000000,    0.7959980,   0.5946441},
                {3,   15,  10,  49,  1.0000000,    0.6437485,   0.6352662},
                {3,   43,  7,   122, 0.7238621,    0.7497578,   0.5158175},
                {10,  2,   23,  4,   1.0000000,    0.6123456,   0.7426754},
                {11,  40,  24,  91,  1.0000000,    0.6261306,   0.5353342},
                {6,   35,  9,   50,  1.0000000,    0.5832668,   0.6397556},
                {36,  14,  45,  17,  1.0000000,    0.5558792,   0.6115855},
                {7,   44,  8,   54,  1.0000000,    0.6594274,   0.5568464},
                {8,   17,  8,   18,  1.0000000,    0.6539915,   0.5815249},
                {24,  21,  22,  20,  1.0000000,    0.6193100,   0.5500174},
                {72,  47,  40,  26,  1.0000000,    0.5582170,   0.5664566},
                {7,   1,   2,   0,   1.0000000,    0.8000000,   1.0000000},
                {52,  4,   13,  1,   1.0000000,    0.7404665,   0.6843913},
                {9,   104, 2,   24,  1.0000000,    0.6485186,   0.6620772},
                {62,  59,  12,  11,  1.0000000,    0.5582062,   0.6208134},
                {77,  11,  14,  2,   1.0000000,    0.6807165,   0.6332436},
                {152, 12,  16,  1,   1.0000000,    0.6495608,   0.7352996},
                {47,  19,  4,   1,   1.0000000,    0.5642943,   0.8195794},
                {5,   23,  0,   1,   1.0000000,    1.0000000,   0.8275862},
                {12,  103, 2,   14,  0.6806391,    0.5352158,   0.7670919},
                {36,  112, 6,   17,  0.8003541,    0.5176500,   0.6802133},
                {68,  49,  22,  38,  7.394647e-03, 0.997971039, 5.358922e-03},
                {10,  6,   0,   2,   1.830065e-01, 1.000000000, 1.830065e-01},
                {1,   2,   0,   0,   1.000000e+00, 1.000000000, 1.000000e+00},
                {2,   0,   147, 28,  1.000000e+00, 1.000000000, 7.078839e-01},
                {14,  0,   39,  2,   1.000000e+00, 1.000000000, 5.521886e-01},
                {50,  34,  11,  17,  8.037209e-02, 0.981348272, 5.025646e-02},
                {32,  89,  1,   47,  1.497951e-04, 0.999996260, 7.030987e-05},
                {1,   0,   0,   0,   1.000000e+00, 1.000000000, 1.000000e+00},
                {2,   16,  3,   33,  1.000000e+00, 0.799903241, 5.455249e-01},
                {30,  2,   49,  5,   1.000000e+00, 0.813226815, 4.787044e-01},
                {2,   16,  0,   37,  1.030303e-01, 1.000000000, 1.030303e-01},
                {27,  7,   0,   1,   2.285714e-01, 1.000000000, 2.285714e-01},
                {9,   16,  13,  61,  9.210764e-02, 0.983433297, 5.395257e-02},
                {8,   25,  4,   96,  1.594311e-03, 0.999824967, 1.594311e-03},
                {41,  0,   78,  3,   5.500610e-01, 1.000000000, 2.889852e-01},
                {4,   4,   4,   3,   1.000000e+00, 0.595182595, 7.855478e-01},
                {35,  2,   7,   5,   6.547709e-03, 0.999592785, 6.547709e-03},
                {3,   0,   4,   2,   5.000000e-01, 1.000000000, 4.166667e-01},
                {15,  1,   39,  29,  7.578115e-03, 0.999669728, 4.395155e-03},
                {21,  6,   22,  10,  5.601280e-01, 0.858027262, 3.160296e-01},
                {43,  4,   38,  4,   1.000000e+00, 0.704775742, 5.779049e-01},
                {10,  22,  3,   30,  3.263025e-02, 0.995376462, 2.605812e-02},
                {0,   1,   3,   25,  1.000000e+00, 0.896551724, 1.000000e+00},
                {5,   2,   8,   12,  2.086957e-01, 0.971014493, 1.608696e-01},
                {15,  2,   33,  19,  7.090668e-02, 0.991349762, 4.714066e-02},
                {5,   33,  11,  77,  1.000000e+00, 0.661556947, 5.634914e-01},
                {141, 30,  6,   10,  2.183596e-04, 0.999975638, 2.183596e-04},
                {2,   27,  0,   5,   1.000000e+00, 1.000000000, 7.237077e-01},
                {39,  17,  18,  23,  1.311194e-02, 0.997100575, 9.707980e-03},
                {16,  71,  2,   22,  3.520339e-01, 0.942040235, 1.952991e-01},
                {120, 4,   64,  9,   1.761090e-02, 0.996919249, 1.570861e-02},
                {2,   0,   10,  1,   1.000000e+00, 1.000000000, 8.461538e-01},
                {2,   49,  0,   9,   1.000000e+00, 1.000000000, 7.203390e-01},
                {5,   6,   1,   8,   1.571207e-01, 0.988080495, 1.191950e-01},
                {59,  7,   27,  8,   1.407360e-01, 0.971773324, 8.983300e-02},
                {17,  5,   54,  44,  9.129567e-02, 0.986577706, 4.470774e-02},
                {11,  14,  8,   28,  9.440502e-02, 0.981336391, 6.408114e-02},
                {21,  13,  37,  31,  5.296223e-01, 0.820703900, 3.114516e-01},
                {11,  45,  2,   21,  3.257843e-01, 0.944178698, 1.985589e-01},
                {22,  26,  21,  40,  2.428231e-01, 0.920237312, 1.557139e-01},
                {16,  21,  26,  72,  9.429818e-02, 0.979914948, 4.977603e-02},
                {3,   2,   6,   15,  3.022195e-01, 0.965521435, 2.081484e-01},
                {4,   0,   71,  3,   1.000000e+00, 1.000000000, 8.520953e-01},
                {46,  111, 0,   7,   1.922482e-01, 1.000000000, 9.481531e-02},
                {16,  28,  1,   5,   6.497876e-01, 0.930300868, 3.236031e-01},
                {2,   1,   30,  11,  1.000000e+00, 0.625490788, 8.239203e-01},
                {89,  3,   57,  14,  1.212660e-03, 0.999913804, 7.165040e-04},
                {11,  8,   2,   3,   6.299172e-01, 0.888198758, 4.145963e-01},
                {4,   15,  18,  94,  5.257813e-01, 0.811511026, 3.990862e-01},
                {1,   88,  0,   63,  1.000000e+00, 1.000000000, 5.855263e-01},
                {8,   2,   42,  18,  7.129647e-01, 0.847793639, 4.093422e-01},
                {43,  6,   102, 46,  8.953214e-03, 0.998338589, 6.044080e-03},
                {1,   2,   3,   30,  3.053221e-01, 0.972549020, 3.053221e-01},
                {9,   24,  0,   1,   1.000000e+00, 1.000000000, 7.352941e-01},
                {30,  21,  35,  68,  5.278451e-03, 0.999056576, 2.892418e-03},
                {16,  1,   38,  7,   4.266585e-01, 0.936247586, 2.919189e-01},
                {41,  3,   121, 24,  1.406549e-01, 0.976162637, 7.961524e-02},
                {29,  64,  16,  81,  2.584046e-02, 0.994821456, 1.331075e-02},
                {5,   12,  8,   37,  3.188872e-01, 0.909291126, 2.512527e-01},
                {1,   0,   20,  4,   1.000000e+00, 1.000000000, 8.400000e-01},
                {9,   66,  0,   25,  1.070050e-01, 1.000000000, 6.602540e-02},
                {0,   1,   12,  46,  1.000000e+00, 0.796610169, 1.000000e+00},
                {50,  12,  20,  25,  1.707701e-04, 0.999980528, 1.115156e-04},
                {4,   27,  1,   26,  3.588399e-01, 0.962918660, 2.224880e-01},
                {32,  22,  3,   14,  4.555450e-03, 0.999654637, 2.744157e-03},
                {60,  103, 0,   4,   2.978506e-01, 1.000000000, 1.651049e-01},
                {104, 17,  2,   1,   3.779262e-01, 0.945073583, 3.779262e-01},
                {46,  19,  34,  40,  3.607078e-03, 0.999185045, 2.561485e-03},
                {17,  4,   147, 23,  5.073296e-01, 0.342041719, 8.462736e-01},
                {85,  6,   61,  17,  6.071913e-03, 0.999150164, 3.824770e-03},
                {12,  45,  0,   16,  5.717603e-02, 1.000000000, 3.846960e-02},
                {18,  31,  17,  67,  4.316402e-02, 0.988303191, 3.103072e-02},
                {56,  23,  51,  57,  1.619071e-03, 0.999677715, 9.501560e-04},
                {54,  107, 0,   2,   1.000000e+00, 1.000000000, 4.458078e-01},
                {41,  13,  38,  20,  3.000901e-01, 0.921851390, 1.586865e-01},
                {23,  52,  5,   34,  4.069462e-02, 0.992207511, 2.768586e-02},
                {0,   2,   7,   32,  1.000000e+00, 0.684146341, 1.000000e+00},
                {3,   9,   8,   65,  1.834241e-01, 0.955185690, 1.834241e-01},
                {16,  10,  10,  12,  3.840257e-01, 0.920214074, 2.052097e-01},
                {4,   26,  8,   156, 9.420457e-02, 0.977118553, 9.420457e-02},
                {5,   15,  19,  10,  8.578508e-03, 0.005763238, 9.991500e-01},
                {90,  40,  2,   10,  5.350621e-04, 0.999959111, 5.350621e-04},
                {20,  4,   10,  6,   1.592189e-01, 0.968160311, 1.322254e-01},
                {3,   3,   0,   5,   1.818182e-01, 1.000000000, 1.212121e-01},
                {36,  36,  23,  41,  1.196834e-01, 0.966288089, 6.943848e-02},
                {6,   55,  4,   74,  3.340991e-01, 0.918383240, 2.303032e-01},
                {0,   126, 1,   35,  2.222222e-01, 0.222222222, 1.000000e+00},
                {2,   0,   0,   0,   1.000000e+00, 1.000000000, 1.000000e+00},
                {29,  2,   0,   1,   9.375000e-02, 1.000000000, 9.375000e-02},
                {1,   3,   4,   29,  4.555227e-01, 0.919978802, 4.555227e-01},
                {68,  6,   36,  10,  5.104677e-02, 0.991400649, 3.292436e-02},
                {0,   8,   0,   2,   1.000000e+00, 1.000000000, 1.000000e+00},
                {52,  10,  67,  26,  1.199103e-01, 0.973191850, 6.342280e-02},
                {5,   0,   103, 5,   1.000000e+00, 1.000000000, 7.941406e-01},
                {61,  3,   42,  2,   1.000000e+00, 0.672788682, 6.807991e-01},
                {0,   33,  3,   96,  5.727246e-01, 0.418643570, 1.000000e+00},
                {10,  57,  6,   50,  5.947466e-01, 0.831278569, 3.388408e-01},
                {16,  0,   8,   0,   1.000000e+00, 1.000000000, 1.000000e+00},
                {61,  31,  16,  45,  1.332855e-06, 0.999999832, 9.930583e-07},
                {0,   5,   1,   17,  1.000000e+00, 0.782608696, 1.000000e+00}
        };
        const size_t nexamples = carray_size(example_data);
        assert(nexamples == 126 && "Number of examples must be right.");
        for(size_t j = 0; j < nexamples; ++j)
        {
//            std::cout << j;
//            for(int i = 0; i < 7; ++i) std::cout << "\t" << example_data[j][i];
//            std::cout << "\n";
            BOOST_CHECK_CLOSE(example_data[j][4],
                              fisher_exact_test_pval_2x2(
                                      int(example_data[j][0]), int(example_data[j][1]),
                                      int(example_data[j][2]), int(example_data[j][3])
                              ), 1e-4);
            BOOST_CHECK_CLOSE(example_data[j][5],
                              fisher_exact_test_pval_2x2(
                                      int(example_data[j][0]), int(example_data[j][1]),
                                      int(example_data[j][2]), int(example_data[j][3]),
                                      FISHER_EXACT::LESS
                              ), 1e-4);
            BOOST_CHECK_CLOSE(example_data[j][6],
                              fisher_exact_test_pval_2x2(
                                      int(example_data[j][0]), int(example_data[j][1]),
                                      int(example_data[j][2]), int(example_data[j][3]),
                                      FISHER_EXACT::GREATER
                              ), 1e-4);
        }
    }

BOOST_AUTO_TEST_SUITE_END()
