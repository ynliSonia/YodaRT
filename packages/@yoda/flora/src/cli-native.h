#pragma once

#include <map>
#include <list>
#include <mutex>
#include <condition_variable>
#include "napi.h"
#include "flora-agent.h"
#include "uv.h"

typedef std::map<std::string, Napi::FunctionReference> SubscriptionMap;

class MsgCallbackInfo {
 public:
  explicit MsgCallbackInfo(Napi::Env e)
      : msgtype(FLORA_MSGTYPE_INSTANT), env(e) {
  }

  /**
  MsgCallbackInfo(const MsgCallbackInfo& o)
      : msgName(o.msgName),
        msg(o.msg),
        msgtype(o.msgtype),
        env(o.env),
        reply(o.reply) {
  }
  */

  std::string msgName;
  std::shared_ptr<Caps> msg;
  uint32_t msgtype;
  Napi::Env env;
  flora::Reply* reply = nullptr;
  bool handled = false;
};

class RespCallbackInfo {
 public:
  std::shared_ptr<Napi::FunctionReference> cbr;
  flora::ResponseArray responses;
};

class HackedNativeCaps {
 public:
  std::shared_ptr<Caps> caps;
};

#define NATIVE_STATUS_CONFIGURED 0x1
#define NATIVE_STATUS_STARTED 0x2
#define ASYNC_HANDLE_COUNT 2

class ClientNative {
 public:
  void handleMsgCallbacks();

  void handleRespCallbacks();

  Napi::Value start(const Napi::CallbackInfo& info);

  Napi::Value subscribe(const Napi::CallbackInfo& info);

  Napi::Value unsubscribe(const Napi::CallbackInfo& info);

  Napi::Value post(const Napi::CallbackInfo& info);

  Napi::Value get(const Napi::CallbackInfo& info);

  Napi::Value genArray(const Napi::CallbackInfo& info);

  void initialize(const Napi::CallbackInfo& info);

  void close();

  void refDown();

 private:
  void msgCallback(const std::string& name, Napi::Env env,
                   std::shared_ptr<Caps>& msg, uint32_t type,
                   flora::Reply* reply);

  void respCallback(std::shared_ptr<Napi::FunctionReference> cbr,
                    flora::ResponseArray& responses);

 private:
  flora::Agent floraAgent;
  SubscriptionMap subscriptions;
  uv_async_t msgAsync;
  uv_async_t respAsync;
  std::list<MsgCallbackInfo> pendingMsgs;
  std::list<RespCallbackInfo> pendingResponses;
  std::mutex cb_mutex;
  std::condition_variable cb_cond;
  Napi::Reference<Napi::Value> thisRef;
  napi_async_context asyncContext = nullptr;
  napi_env thisEnv = 0;
  // CONFIGURED
  // STARTED
  uint32_t status = 0;
  uint32_t asyncHandleCount = ASYNC_HANDLE_COUNT;
};

class NativeObjectWrap : public Napi::ObjectWrap<NativeObjectWrap> {
 public:
  explicit NativeObjectWrap(const Napi::CallbackInfo& info);

  ~NativeObjectWrap();

  static Napi::Object Init(Napi::Env env, Napi::Object exports);

 private:
  Napi::Value start(const Napi::CallbackInfo& info);

  Napi::Value subscribe(const Napi::CallbackInfo& info);

  Napi::Value unsubscribe(const Napi::CallbackInfo& info);

  Napi::Value close(const Napi::CallbackInfo& info);

  Napi::Value post(const Napi::CallbackInfo& info);

  Napi::Value get(const Napi::CallbackInfo& info);

  Napi::Value genArray(const Napi::CallbackInfo& info);

 private:
  ClientNative* thisClient = nullptr;
};
