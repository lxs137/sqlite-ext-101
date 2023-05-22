## Getting Started

```sql
create table articles(title text, title_vec blob, readings integer);
insert into articles(title, title_vec, readings) values("hello world", vector_json_to_blob("[1.1, 2.2, 1.2]"), 42);
select title, vector_blob_to_text(title_vec), readings from articles;
```