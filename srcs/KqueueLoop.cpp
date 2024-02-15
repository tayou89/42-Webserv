#include "../include/KqueueLoop.hpp"

KqueueLoop::KqueueLoop(std::map<int, IServer *> serverList)
    : _serverList(serverList) {
  this->_kqueue = kqueue();
  if (this->_kqueue == -1)
    exit(1); // exit when error occurs
  fcntl(_kqueue, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

KqueueLoop::KqueueLoop() {}

KqueueLoop::~KqueueLoop() {}

KqueueLoop::KqueueLoop(const KqueueLoop &ref) { static_cast<void>(ref); }

KqueueLoop &KqueueLoop::operator=(const KqueueLoop &ref) {
  if (this == &ref)
    return (*this);
  return (*this);
}

void KqueueLoop::initServerSocket() {
  // register _serverList at kqueue
  std::map<int, IServer *>::iterator iter = _serverList.begin();

  for (; iter != _serverList.end(); iter++) {
    // iter->second->getListenSocket 으로 반환된 descriptor를
    // Event 객체로 변환하여 _changeList에 등록함
  }
}

void KqueueLoop::newEvent(Kevent event) {
  this->_changeList.push_back(event.getEvent());
}

void KqueueLoop::run() {
  int eventCount;
  struct kevent newEvents[8];
  struct kevent *currentEvent;

  while (1) {
    eventCount = kevent(_kqueue, &_changeList[0], _changeList.size(), newEvents,
                        8, NULL);
    if (eventCount == -1)
      exit(1);           // kevent error exit
    _changeList.clear(); // clear list of new register events

    for (int idx = 0; idx < eventCount; idx++) {
      currentEvent = &newEvents[idx];
      if (currentEvent->flags & EV_ERROR)
        continue; // skip the error event

      /* EVENT THROWER*/
      /* 서버객체를 저장하지 않고 event 단위로 행동을 정의할경우 listenSocket에
       * 대한 판단이 필수적인지에 대해서 다시한번 생각해봐야함 */
      if (_serverList.find(currentEvent->ident) != _serverList.end()) {
        // new client accept
        // 새로 accept 된 socket은 _eventList에 등록한다.
        // int newSocket = _serverList[currentEvent->ident]->acceptClient();
        // _eventList[newSocket] =
        // new ClientStat(IServer *currentServer, int newSocket);
        // !!!!!!leak warning!!!!!! 연결 해제시 반드시 할당된 메모리도 해제할 것
      } else {
        /* eventloop는 발생한 이벤트를 서버에 던져주기만 하고 동작에 대한 내부
         * 구현은 이벤트 객체가 가리키는 서버에서 내부적으로 처리 */

        /* 서버 내부 동작 구현 */
        // timeout exception 먼저 처리 (Event객체 내부구현)
        // client socket event occurs (READ or WRITE)
        // One Action per One Event
        // return type register new event?
        // Udata *ptr = static_cast<Udata *>(currentEvent->udata) 이용?
        // Server class 에서 이벤트 처리하려면 udata 이용하는편이 좋을수도?
        // 이벤트 객체에 *udata를 server class instance로 활용하면 편할수도?
        // 이벤트 실행을 구현하면서 둘중에 용의한 방법을 취사선택 해야될듯
        // read size가 0이면 클라이언트 소켓의 연결이 종료된 상태
        // fd(ident)를 index값으로 가지는 map에 fd별로 Kevent객체를 저장
        // Kevent instance에 서버가 저장되어 있으므로 행동을 정의하기 용이하고
        // fd의 현재 상태를 표시하기 좋음 (timeout, readable, eof 등등)
      }
    }
  }
}
