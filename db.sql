show databases;
create database stocks_cn;
use database stocks_cn;
create table stock_info (
	code mediumint not null, 
	name char(10) not null, 
	businesstype char(10) not null, 
	primary key (code)) 
comment = "Base information of Stocks";
show tables;
create table stock_real_601118 (
	id int not null primary key auto_increment, 
	code_name varchar(20) not null, 
	open double not null, 
	close double not null,
	curprice double not null,
	high double not null,
	low double not null,
	bidprice double not null,
	auctionprice double not null,
	volume double not null,
	amount double not null,
	buy1num double,
	buy1price double,
	buy2num double,
	buy2price double,
	buy3num double,
	buy3price double,
	buy4num double,
	buy4price double,
	buy5num double,
	buy5price double,
	sell1num double,
	sell1price double,
	sell2num double,
	sell2price double,
	sell3num double,
	sell3price double,
	sell4num double,
	sell4price double,
	sell5num double,
	sell5price double,
	time_date char(20),
	time_acc char(20),
	additioninfo char(10))
comment = "Real time information of specific stock";
describe stock_info;
describe stock_real_601118;
alter table stock_real_601118 modify time_date date not null;
alter table stock_real_601118 modify time_acc time not null;
alter table stock_real_601118 add additioninfo char(10);
alter database stocks_cn character set 'utf8' collate 'utf8_unicode_ci';
alter table stock_real_601118 modify code_name char(10) CHARACTER SET utf8 COLLATE utf8_general_ci;
)
select concat('drop table ', table_name, ';') from information_schema.tables where table_name like '%_trade';
