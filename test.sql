drop table if exists cq_item; 
create table cq_item
(
	id varchar(10) primary key,
	chk_sum varchar(10)
);

insert into cq_item values(35787530,12345678);