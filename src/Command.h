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
#ifndef _D_COMMAND_H_
#define _D_COMMAND_H_

#include "common.h"
#include <stdint.h>

namespace aria2 {

class Logger;

typedef int32_t CommandUuid;

typedef int64_t cuid_t;

class Command {
public:
  enum STATUS {
    STATUS_ALL,
    STATUS_INACTIVE,
    STATUS_ACTIVE,
    STATUS_REALTIME,
    STATUS_ONESHOT_REALTIME
  };
private:
  CommandUuid uuid;
  static int32_t uuidGen;
  STATUS _status;

  cuid_t _cuid;
  Logger* _logger;

  bool _readEvent;
  bool _writeEvent;
  bool _errorEvent;
  bool _hupEvent;
protected:
  Logger* getLogger() const
  {
    return _logger;
  }

  bool readEventEnabled() const
  {
    return _readEvent;
  }

  bool writeEventEnabled() const
  {
    return _writeEvent;
  }

  bool errorEventEnabled() const
  {
    return _errorEvent;
  }

  bool hupEventEnabled() const
  {
    return _hupEvent;
  }
public:
  Command(cuid_t cuid);

  virtual ~Command() {}

  virtual bool execute() = 0;

  cuid_t getCuid() const { return _cuid; }

  const CommandUuid& getUuid() const { return uuid; }

  void setStatusActive() { _status = STATUS_ACTIVE; }

  void setStatusInactive() { _status = STATUS_INACTIVE; }

  void setStatusRealtime() { _status = STATUS_REALTIME; }

  void setStatus(STATUS status);

  bool statusMatch(Command::STATUS statusFilter) const
  {
    return statusFilter <= _status;
  }

  void transitStatus();

  void readEventReceived();

  void writeEventReceived();

  void errorEventReceived();

  void hupEventReceived();

  void clearIOEvents();
};

} // namespace aria2

#endif // _D_COMMAND_H_
