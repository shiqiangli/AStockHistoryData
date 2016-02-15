#!/bin/sh
var=`date "+%Y%m%d"`
echo "Database backup..."
mysqldump -uroot -p19870923 stocks_cn > /home/lishiqiang/stocks.rich/db.backup/$var.sql
tar czvf /home/lishiqiang/stocks.rich/db.backup/$var.sql.tar.gz /home/lishiqiang/stocks.rich/db.backup/$var.sql
rm /home/lishiqiang/stocks.rich/db.backup/$var.sql
