/************************************************************************************
   Copyright (C) 2020 MariaDB Corporation AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not see <http://www.gnu.org/licenses>
   or write to the Free Software Foundation, Inc.,
   51 Franklin St., Fifth Floor, Boston, MA 02110, USA
*************************************************************************************/


#ifndef _MARIADBSTATEMENT_H_
#define _MARIADBSTATEMENT_H_

#include <regex>
#include <map>
#include <mutex>

//#include "MariaDbConnection.h"

#include "ResultSet.h"
#include "Statement.h"
#include "Consts.h"
#include "Charset.h"

namespace sql
{
namespace mariadb
{
class MariaDbConnection;

class MariaDbStatement : public Statement
{
  static std::regex identifierPattern ; /*Pattern.compile("[0-9a-zA-Z\\$_\\u0080-\\uFFFF]*",Pattern.UNICODE_CASE |Pattern.CANON_EQ)*/
  static std::regex escapePattern ; /*Pattern.compile("[\u0000'\"\b\n\r\t\u001A\\\\]")*/
  static const std::map<std::string,std::string> mapper;
  static Shared::Logger logger ; /*LoggerFactory.getLogger(MariaDbStatement)*/

  /* We don't want copy constructing*/
  MariaDbStatement(const MariaDbStatement& other) = delete;

protected:
  MariaDbConnection* connection;
  //TODO: possibly it is better to make it weak ptr, and check if it's still available, and gracefully throw exception otherwise
  Protocol* protocol;
  const Shared::mutex lock;
  int32_t resultSetScrollType;
  int32_t resultSetConcurrency;
  const Shared::Options options;
  bool canUseServerTimeout;
  Shared::ExceptionFactory exceptionFactory;

  volatile bool closed ; /*false*/
  int32_t queryTimeout;
  int64_t maxRows;
  Shared::Results results;
  int32_t fetchSize;
  volatile bool executing;

private:
#ifdef MAYBE_IN_BETA
  ScheduledExecutorService timeoutScheduler;
#endif
  bool warningsCleared;
  bool mustCloseOnCompletion ; /*false*/
  std::vector<SQLString> batchQueries;
#ifdef MAYBE_IN_BETA
  Future<?>timerTaskFuture;
#endif
  bool isTimedout;
  int32_t maxFieldSize;

public:
  MariaDbStatement(MariaDbConnection* connection, int32_t resultSetScrollType, int32_t resultSetConcurrency, Shared::ExceptionFactory& factory);
  MariaDbStatement* clone(MariaDbConnection* connection);
  virtual ~MariaDbStatement();
  // Was protected, and public is not so good
  void setTimerTask(bool isBatch);
  Statement* setResultSetType(int32_t rsType);

protected:
  void executeQueryPrologue(bool isBatch);
private:
  void stopTimeoutTask();
  SQLException handleFailoverAndTimeout(SQLException& sqle);
public://protected:
  void executeEpilogue();
  void executeBatchEpilogue();
  SQLException executeExceptionEpilogue(SQLException& sqle);
  BatchUpdateException executeBatchExceptionEpilogue(SQLException& initialSqle, std::size_t size);
private:
  bool executeInternal(const SQLString& sql,int32_t fetchSize,int32_t autoGeneratedKeys);
public:
  SQLString enquoteLiteral(const SQLString& val);
  SQLString enquoteIdentifier(const SQLString& identifier,bool alwaysQuote);
  bool isSimpleIdentifier(const SQLString& identifier);
  SQLString enquoteNCharLiteral(const SQLString& val);
private:
  SQLString getTimeoutSql(const SQLString& sql);
public:
  bool testExecute(const SQLString& sql, const Charset& charset);

  bool execute(const SQLString& sql);
  bool execute(const SQLString& sql,int32_t autoGeneratedKeys);
  bool execute(const SQLString& sql,int32_t* columnIndexes);
  bool execute(const SQLString& sql,const SQLString* columnNames);
  ResultSet* executeQuery(const SQLString& sql);
  int32_t executeUpdate(const SQLString& sql);
  int32_t executeUpdate(const SQLString& sql,int32_t autoGeneratedKeys);
  int32_t executeUpdate(const SQLString& sql,int32_t* columnIndexes);
  int32_t executeUpdate(const SQLString& sql,const SQLString* columnNames);
  int64_t executeLargeUpdate(const SQLString& sql);
  int64_t executeLargeUpdate(const SQLString& sql,int32_t autoGeneratedKeys);
  int64_t executeLargeUpdate(const SQLString& sql,int32_t* columnIndexes);
  int64_t executeLargeUpdate(const SQLString& sql,SQLString* columnNames);
  void close();
  int32_t getMaxFieldSize();
  void setMaxFieldSize(int32_t max);
  int32_t getMaxRows();
  void setMaxRows(int32_t max);
  int64_t getLargeMaxRows();
  void setLargeMaxRows(int64_t max);
  void setEscapeProcessing(bool enable);
  int32_t getQueryTimeout();
  void setQueryTimeout(int32_t seconds);
  void setLocalInfileInputStream(std::istream* inputStream);
  void cancel();
  SQLWarning* getWarnings();
  void clearWarnings();
  void setCursorName(const SQLString& name);
  Connection* getConnection();
  ResultSet* getGeneratedKeys();
  int32_t getResultSetHoldability();
  bool isClosed();
  bool isPoolable();
  void setPoolable(bool poolable);
  ResultSet* getResultSet();
  int32_t getUpdateCount();
  int64_t getLargeUpdateCount();

//protected:
  void skipMoreResults();

public:
  bool getMoreResults();
  bool getMoreResults(int32_t current);
  int32_t getFetchDirection();
  void setFetchDirection(int32_t direction);
  int32_t getFetchSize();
  void setFetchSize(int32_t rows);
  int32_t getResultSetConcurrency();
  int32_t getResultSetType();
  void addBatch(const SQLString& sql);
  void clearBatch();
  sql::Ints* executeBatch();
  sql::Longs* executeLargeBatch();

private:
  void internalBatchExecution(int32_t size);

public:
  void closeOnCompletion();
  bool isCloseOnCompletion();
  void checkCloseOnCompletion(ResultSet* resultSet);
//protected:
  void checkClose();
public:
  int64_t getServerThreadId();
  /* TODO: not quite nice to have these public */
  Shared::Results& getInternalResults();
  void setInternalResults(Results* newResults);
  void setExecutingFlag(bool _set= true);
  void markClosed();
  //Shared::Protocol& getProtocol() { return protocol; }
  //Shared::Options& getOptions() { return options; }
  };
}
}
#endif
