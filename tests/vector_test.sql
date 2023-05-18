.bail "on"
.load out/vector

select vector_version();

select vector_to_blob("[1, 2, 3.1]");
