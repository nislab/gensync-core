#ifndef COMMSOCKETUDPTEST_H
#define COMMSOCKETUDPTEST_H

#include <cppunit/extensions/HelperMacros.h>

class CommSocketUDPTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(CommSocketUDPTest);

    CPPUNIT_TEST(GetSocketInfo);
    CPPUNIT_TEST(SocketSendAndReceiveTest);

    CPPUNIT_TEST_SUITE_END();

public:
    CommSocketUDPTest();
    ~CommSocketUDPTest() override;

    void setUp() override;
    void tearDown() override;

private:
	/**
 	* Tests getName and getHost functions ensuring that constructor is working as intended
 	*/
    void GetSocketInfo();

	/**
 	* Tests sending and receiving a string of characters through a socket
	* @note: This test reports a failure if it has not completed after WAIT_TIME seconds (1 second)
 	*/
    void SocketSendAndReceiveTest();

};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( CommSocketUDPTest, CommSocketUDPTest );

#endif /* COMMSOCKETUDPTEST_H */
