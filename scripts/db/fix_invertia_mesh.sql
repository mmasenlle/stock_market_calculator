--fix the invertia mesh due to incoherences in numbers (sometimes nn.nn and sometimes nn,nn)

delete from feeder_value_data using feeder_value_ids, statistics_of_month
 where feeder_value_data.date >= '20101101' and statistics_of_month.date = '20101001'
  and feeder_value_data.value = feeder_value_ids.source_id
  and feeder_value_ids.ccltor_id = statistics_of_month.value 
  and feeder_value_data.price > (statistics_of_month.max_price * 1.3);

delete from statistics_of_day where date >= '20101101';
delete from trends where date >= '20101101';
delete from trends_acum where date >= '20101101';
delete from wealth_sday where date >= '20101101';
delete from wealth_trends where date >= '20101101';
delete from wealth_trends_acum where date >= '20101101';
