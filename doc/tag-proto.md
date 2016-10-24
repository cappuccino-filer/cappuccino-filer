= Protocol of (/api/tag, and /api/filetag)

== Tag (entry: /api/tag)
=== Creation
Request:
```
{
	"cat" : "create",
	"name" : <string>
}
```

Response
```
{
	"cat" : "create",
	"result" : "OK",
	"name" : <string>,
	"tagid" : <integer>
}
```

=== List Tag
Request:
```
{
	"cat" : "list",
	"offset" : <integer>
	"limit" : <integer>
}
```

Response
```
{
	"cat" : "list",
	"result" : "OK",
	"tags" : [ { "id" : <integer> , "name": <string> }, ...]
}
```

=== Get ID from name
Request:
```
{
	"cat" : "name2id",
	"name" : <string>,
	"options" : {
		"create_if_not_exits" : <boolean>
	},
	"constraints" : [
		{
			"cat" : "tagged_with"
			"tags" : [list of tags]
		},
		... // more constraint objects
	]
}
```

Response
```
{
	"cat" : "name2id",
	"result" : "OK"/"Not found", 
	"tagid" : <list of integers>
}
```

=== Create/Find/Overwrite tag-tag relationship

Request:
```
{
	"cat" : "tagtag",
	"taggee" : <integer>, // negative value means "any"
	"tagger" : <integer>, // negative value means "any"
	"probability" : <real> // value must be < -1 to enable find function
}
```

Response
```
{
	"cat" : "tagtag",
	"result" : "OK"/"Tag not found"/"Relationship not found",
	"rels" : [
		{ "relid"  : ... ,
		  "taggee" : ... ,
		  "tagger" : ... ,
		  "probability" : ...
		},
	] //<list of tag-tag relation objects>,
}
```

=== Create/Find/Overwrite tag-relationship relationship

Request:
```
{
	"cat" : "tagrel",
	"taggee" : <integer>,
	"tagger" : <integer>,
	"probability" : <real> // set to values < -1 for find function
}
```

Response
```
{
	"cat" : "tagrel",
	"result" : "OK"/"Tag not found"/"Relationship not found" 
	"relid" : <integer>
}
```

=== Deletion
Request:
```
{
	"cat" : "delete_tag",
	"tagid" : [list of tag ids]
}
```

Response
```
{
	"cat" : "delete_tag",
	"result" : "OK"/"Tag not found",
	"tagid" : [list of tags with errors]
}
```

Deletion of relationships is similar, just replace "tag" with "rel".

== File's tag (entry: /api/filetag)

// TODO
