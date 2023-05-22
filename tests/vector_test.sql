.bail "on"
.load out/vector

select vector_version();

select vector_json_to_text("[1, 2, 3.1]");

create table foos(title text, title_vec blob);

insert into foos(title, title_vec) values("ha", vector_json_to_blob("[1, 2, 3.1]"));
insert into foos(title, title_vec) values("hb", vector_json_to_blob("[1, 2.1, 3.1]"));
insert into foos(title, title_vec) values("hc", vector_json_to_blob("[1, 2.2, 3.1]"));
insert into foos(title, title_vec) values("hf", vector_json_to_blob("[1, 3, 3.1]"));

select title, vector_blob_to_text(title_vec) from foos;
