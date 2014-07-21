#!/usr/bin/python

import sys
import os
import glob
import smtplib
from email.MIMEMultipart import MIMEMultipart
from email.MIMEBase import MIMEBase
from email.MIMEText import MIMEText
from email.Utils import COMMASPACE, formatdate
from email import Encoders

def SendEmail(mail_from, mail_to, mail_subject, message):
    mail_from='fm_monitor@fmkj.net'
    mail_smtp = 'mta1.fmkj.net'
    mail_user = mail_from
    mail_to_list = mail_to.split(",")

    msg = MIMEMultipart()
    msg['From'] = mail_from
    msg['To'] = mail_to
    msg['Date'] = formatdate(localtime=True)
    msg['Subject'] = mail_subject

    msg_html = MIMEText(message)
    msg.attach(msg_html)

    print 'connect smtp server...'
    smtp = smtplib.SMTP(mail_smtp)
    print 'begin to send email...'
    smtp.sendmail(mail_from, mail_to_list, msg.as_string() )
    smtp.close()
    print 'email sent! Now Exit!'

if __name__ == '__main__':
    if len(sys.argv) < 5:
        print 'usage: python ' + sys.argv[0] + ' from to subject message'
    else:
        SendEmail(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])

