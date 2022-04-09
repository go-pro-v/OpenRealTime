
#include <RTNTPClient.h>


RTNTPClient::RTNTPClient(String ntp_server)
{
	server_name = ntp_server;
}

void RTNTPClient::start()
{
	Serial.println ( "begin Adjust local clock" );
	timeClient.setPoolServerName(server_name.c_str());
	timeClient.begin();
	delay ( 1000 );
	while (!timeClient.forceUpdate())
	{
		Serial.println ( "fails to adjust local clock" );
		delay(1000);
	}

	Serial.println ( "Adjust local clock" );
	setTime(timeClient.getEpochTime());
}
void RTNTPClient::run()
{
	if(timeinfo != 0)
	{
		previousSecond = timeinfo->tm_sec;
	}
	timeClient.update();
	time_t rawTime = now();
	timeinfo = localtime(&rawTime);
	if(timeinfo != 0)
	{
		currentSecond = timeinfo->tm_sec;
	}
}

void RTNTPClient::setOffset(int offset)
{
	timeClient.setTimeOffset(offset);
	setTime(timeClient.getEpochTime());
}
bool RTNTPClient::changed()
{
	return previousSecond != currentSecond;
}
char* RTNTPClient::toChar()
{
	return asctime(timeinfo);
}
int RTNTPClient::second()
{
	return timeinfo->tm_sec;
}
int RTNTPClient::minute()
{
	return timeinfo->tm_min;
}
int RTNTPClient::hour()
{
	return timeinfo->tm_hour;
}
int RTNTPClient::dayOfMonth()
{
	return timeinfo->tm_mday + 1;
}
int RTNTPClient::month()
{
	return timeinfo->tm_mon + 1;
}
int RTNTPClient::year()
{
	return timeinfo->tm_year + 1900;
}
int RTNTPClient::dayOfWeek()
{
	return timeinfo->tm_wday + 1;
}
int RTNTPClient::dayOfYear()
{
	return timeinfo->tm_yday + 1;
}