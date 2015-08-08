#include <stdio.h>
#include <QCoreApplication>
#include <QDebug>
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include "logger.h"
#include "pref.h"

namespace {
static FILE* flog()
{
	return Pref::instance()->get_flog();
}

void message_to_file(QtMsgType type, const QMessageLogContext &ctx, const QString &msg)
{
	QByteArray localMsg = msg.toLocal8Bit();
	FILE* file = flog();
	const char* prefix = "";
	switch (type) {
		case QtDebugMsg:
			prefix = "Debug";
			break;
		case QtWarningMsg:
			prefix = "Warning";
			break;
		case QtCriticalMsg:
			prefix = "Critical";
			break;
		case QtFatalMsg:
			prefix = "Fatal";
			break;
	}
	QByteArray isodate = QDateTime::currentDateTime().toString(Qt::ISODate).toLocal8Bit();
	if (ctx.file)
		fprintf(file, "[%s] %s: %s (%s:%u)\n",
			isodate.constData(),
			prefix,
			localMsg.constData(),
			ctx.function,
			ctx.line);
	else
		fprintf(file, "[%s] %s: %s\n",
			isodate.constData(),
			prefix,
			localMsg.constData());
	if (type == QtFatalMsg)
		abort();
}

struct log_closer {
	~log_closer()
	{
		if (flog() != stderr) {
			fclose(flog());
			qInstallMessageHandler(nullptr);
		}
	}
};
}

void logger::init()
{
	//qSetMessagePattern("%{file}(%{line}): %{message}");
	qInstallMessageHandler(message_to_file);
	static log_closer closer;
}

void logger::dump()
{
	fflush(flog());
}
