/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#include "DHTGetPeersCommand.h"
#include "DHTTaskQueue.h"
#include "DHTTaskFactory.h"
#include "DHTTask.h"
#include "DownloadEngine.h"
#include "RequestGroup.h"
#include "DHTNode.h"
#include "DHTNodeLookupEntry.h"
#include "BtRuntime.h"
#include "PeerStorage.h"
#include "Peer.h"
#include "Logger.h"
#include "bittorrent_helper.h"
#include "DownloadContext.h"
#include "wallclock.h"
#include "RequestGroupMan.h"
#include "FileAllocationEntry.h"
#include "CheckIntegrityEntry.h"
#include "ServerStatMan.h"

namespace aria2 {

DHTGetPeersCommand::DHTGetPeersCommand(cuid_t cuid,
                                       RequestGroup* requestGroup,
                                       DownloadEngine* e):
  Command(cuid),
  _requestGroup(requestGroup),
  _e(e),
  _numRetry(0),
  _lastGetPeerTime(0)
{
  _requestGroup->increaseNumCommand();
}

DHTGetPeersCommand::~DHTGetPeersCommand()
{
  _requestGroup->decreaseNumCommand();
}

bool DHTGetPeersCommand::execute()
{
  if(_btRuntime->isHalt()) {
    return true;
  }
  if(_task.isNull() &&
     ((_numRetry > 0 &&
       _lastGetPeerTime.difference(global::wallclock) >= (time_t)_numRetry*5) ||
      _lastGetPeerTime.difference(global::wallclock) >= GET_PEER_INTERVAL)) {
    if(getLogger()->debug()) {
      getLogger()->debug("Issuing PeerLookup for infoHash=%s",
                         bittorrent::getInfoHashString
                         (_requestGroup->getDownloadContext()).c_str());
    }
    _task = _taskFactory->createPeerLookupTask
      (_requestGroup->getDownloadContext(), _btRuntime, _peerStorage);
    _taskQueue->addPeriodicTask2(_task);
  } else if(!_task.isNull() && _task->finished()) {
    _lastGetPeerTime = global::wallclock;
    if(_numRetry < MAX_RETRIES && _btRuntime->lessThanMinPeers()) {
      ++_numRetry;
    } else {
      _numRetry = 0;
    }
    _task.reset();
  }

  _e->addCommand(this);
  return false;
}

void DHTGetPeersCommand::setTaskQueue(const SharedHandle<DHTTaskQueue>& taskQueue)
{
  _taskQueue = taskQueue;
}

void DHTGetPeersCommand::setTaskFactory(const SharedHandle<DHTTaskFactory>& taskFactory)
{
  _taskFactory = taskFactory;
}

void DHTGetPeersCommand::setBtRuntime(const SharedHandle<BtRuntime>& btRuntime)
{
  _btRuntime = btRuntime;
}

void DHTGetPeersCommand::setPeerStorage(const SharedHandle<PeerStorage>& ps)
{
  _peerStorage = ps;
}

} // namespace aria2
