.bail "on"
.load out/tick

select tick_version();

select tick("hello");
select tick_multi("hi", 3);
