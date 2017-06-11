#include <iostream>
#include <tr1/memory>
#include <queue>
#include <windows.h>

using std::cout;
using std::endl;

class Request{};

class StopperPIMPL {
    bool shouldStop;
    HANDLE hMutex;
public:
    StopperPIMPL():shouldStop(false) {
        hMutex = CreateMutex(NULL, FALSE, NULL);
        if (!hMutex)
            cout << "CreateMutex Error: " << GetLastError() << endl;
    }
    ~StopperPIMPL() {
        CloseHandle(hMutex);
    }
    void stop() {
        WaitForSingleObject(hMutex, INFINITE);
        shouldStop = true;
        ReleaseMutex(hMutex);
    }
    bool check() {
        WaitForSingleObject(hMutex, INFINITE);
        bool tmp = shouldStop;
        ReleaseMutex(hMutex);
        return tmp;
    }
};

class Stopper {
    std::tr1::shared_ptr<StopperPIMPL> data;
public:
    Stopper():data(new StopperPIMPL){}
    void stop() {data->stop();}
    bool check() {return data->check();}
};

class RequestQueue {
    std::queue<Request*> queue;
    HANDLE hMutex;
public:
    RequestQueue() {
        hMutex = CreateMutex(NULL, FALSE, NULL);
        if (!hMutex)
            cout << "CreateMutex Error: " << GetLastError() << endl;
    }
    ~RequestQueue() {
        CloseHandle(hMutex);
    }
    void push(Request* req) {
        WaitForSingleObject(hMutex, INFINITE);
        queue.push(req);
        ReleaseMutex(hMutex);
    }
    Request* pop() {
        WaitForSingleObject(hMutex, INFINITE);
        Request* req = NULL;
        if (!queue.empty()) {
            req = queue.front();
            queue.pop();
        }
        ReleaseMutex(hMutex);
        return req;
    }
};

struct SharedObjects {
    Stopper stopper;
    RequestQueue waitingRequests;
};

Request* GetRequest(Stopper stopSignal) throw() {
    // some code
    return stopSignal.check() ? NULL : new Request;
}

void ProcessRequest(Request*, Stopper stopSignal) throw() {
    if(stopSignal.check()) return;
    // some code
}

DWORD WINAPI StartGetThread(LPVOID shObjParam) {
    cout << "start GetThread" << endl;
    SharedObjects& shObj = *reinterpret_cast<SharedObjects*>(shObjParam);
    while(!shObj.stopper.check()) {
        Request* req = GetRequest(shObj.stopper);
        if(!req) continue;
        shObj.waitingRequests.push(req);
    }
    cout << "end GetThread" << endl;
    return 0;
}

DWORD WINAPI StartProcessThread(LPVOID shObjParam) {
    cout << "start ProcessThread" << endl;
    SharedObjects& shObj = *reinterpret_cast<SharedObjects*>(shObjParam);
    while(!shObj.stopper.check()) {
        Request* req = shObj.waitingRequests.pop();
        if(!req) continue;
        ProcessRequest(req, shObj.stopper);
        delete req;
    }
    cout << "end ProcessThread" << endl;
    return 0;
}

int main() {
    cout << "start MainThread" << endl;
    const int PROCESS_THREAD_COUNT = 5;
    const int TIME_TO_WORK = 10;

    SharedObjects shObj;
    HANDLE hThreads[PROCESS_THREAD_COUNT+1];

    hThreads[0] = CreateThread(NULL, 0, StartGetThread, &shObj, 0, NULL);
    if (!hThreads[0]) {
        cout << "CreateThread Error: " << GetLastError() << endl;
        return 1;
    }
    for (int i = 1; i < PROCESS_THREAD_COUNT + 1; ++i) {
        hThreads[i] = CreateThread(NULL, 0, StartProcessThread, &shObj, 0, NULL);
        if (!hThreads[i]) {
            cout << "CreateThread Error: " << GetLastError() << endl;
            return 1;
        }
    }

    Sleep(TIME_TO_WORK * 1000);
    shObj.stopper.stop();
    cout << "signal Stopper" << endl;

    WaitForMultipleObjects(PROCESS_THREAD_COUNT + 1, hThreads, true, INFINITE);
    for (int i = 0; i < PROCESS_THREAD_COUNT + 1; ++i)
        CloseHandle(hThreads[i]);

    cout << "end MainThread" << endl;
    return 0;
}
