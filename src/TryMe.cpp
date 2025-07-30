#include <iostream>
#include <GenSync/Syncs/GenSync.h>

int main() {
  // BUILD the first host
  GenSync host1 = GenSync::Builder().
    setProtocol(GenSync::SyncProtocol::CPISync). // CPISync protocol
    setComm(GenSync::SyncComm::socketudp).		 // communicate over network sockets
    setMbar(5).					 // required parameter for CPISync
    build();

  // BUILD the second host
  GenSync host2 = GenSync::Builder().
    setProtocol(GenSync::SyncProtocol::CPISync).
    setComm(GenSync::SyncComm::socketudp).
    setMbar(5).
    build();

  // ADD elements to each host
  // ... host 1
  host1.addElem(make_shared<DataObject>('a')); // DataObject containing a character 'a'
  host1.addElem(make_shared<DataObject>('b'));
  host1.addElem(make_shared<DataObject>('c'));

  // ... host 2
  host2.addElem(make_shared<DataObject>('b'));
  host2.addElem(make_shared<DataObject>('d'));

  // FORK into two processes
  if (fork()) {
      // ... PARENT process
      host1.clientSyncBegin(0);		     // set up the 0-th synchronizer and connect to a server
      cout << "host 1 now has ";
      for (auto &i: host1.dumpElements())    // print out the elements at host 1
	cout << i << " ";
      cout << endl;
    }
    else {
      // ... CHILD process
      host2.serverSyncBegin(0);		      // set up the 0-th synchronizer and wait for connections
      cout << "host 2 now has ";
      for (auto &i: host2.dumpElements())     // print out the elements at host 2
	cout << i << " ";
      cout << endl;
    }

  }