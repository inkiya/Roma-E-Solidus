// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 500;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        ( 0,   uint256("0x000005dab0c470667d1c87d42e5fd7d805ccfc2fa950215c14610eea4eb5c882") ) // genesis
		(29443,	uint256("0x000001e10382362eb61e610b3cce6b0a69852c09e9cb8d3b9dcc0c59fc594e49 "))
		(54364,	uint256("0x7f48fe11a37dab810a4245e4acf9272981da5683978e2f1fab84a3767332d72e"))
		(91699, uint256("0x0000041da91290856347aba577aa239c321e8c0f7aaee4c85709a85bf927960d"))
		(122082, uint256("0xdd70fa52cfc3e2dbba2c1098bf40b5c8c8d525520ec9fe55e721e09f837a9a35"))
		(177096, uint256("0x01a6ff24f62501810c566a8f8c1c7e3d54fef298cdffafeb32c3759e80eff477"))
		(177099, uint256("0xc35a26b7e8e68fd9207010e7f1dc61ac5e63632784f012c71507139cbae49066"))
		(201538, uint256("0x410e0d54830ae7cf4d12bda0946824e3599d2a757d1047884f60e0bfb3194b12"))
		(206839, uint256("0xd688d7024d0d2bc6886ebd9245319aa2013fad24cfcceaf235207b94d3f6b139"))
		//( 0000,   uint256("0x") )
		//( 0000,   uint256("0x") )
    ;


    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}
