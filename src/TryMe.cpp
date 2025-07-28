#include <iostream>
#include <GenSync/Syncs/GenSync.h>

int main() {
  // FORK into two processes
  if (fork()) {
      // BUILD the first host
      GenSync host1 = GenSync::Builder().
      setProtocol(GenSync::SyncProtocol::CPISync). // CPISync protocol
      setComm(GenSync::SyncComm::socket).		 // communicate over network sockets
      setContainer(GenSync::ContainerType::SQLite, "temp", "temp", true).
      setMbar(5).					 // required parameter for CPISync
      build();

      // ... host 1
      host1.addElem(make_shared<DataObject>('a')); // DataObject containing a character 'a'
      host1.addElem(make_shared<DataObject>('b'));
      host1.addElem(make_shared<DataObject>('c'));

      // ... PARENT process
      host1.clientSyncBegin(0);		     // set up the 0-th synchronizer and connect to a server
      cout << "host 1 now has ";
      for (auto &i: host1.dumpElements())    // print out the elements at host 1
	    cout << i << " ";
      cout << endl;
    }
    else {
      // BUILD the second host
      GenSync host2 = GenSync::Builder().
      setProtocol(GenSync::SyncProtocol::CPISync).
      setComm(GenSync::SyncComm::socket).
      setContainer(GenSync::ContainerType::SQLite, "temp2", "temp2", true).
      setMbar(5).
      build();
      
      // ... host 2
      host2.addElem(make_shared<DataObject>('b'));
      host2.addElem(make_shared<DataObject>('d'));

      // ... CHILD process
      host2.serverSyncBegin(0);		      // set up the 0-th synchronizer and wait for connections
      cout << "host 2 now has ";
      for (auto &i: host2.dumpElements())     // print out the elements at host 2
	    cout << i << " ";
      cout << endl;
    }

  }