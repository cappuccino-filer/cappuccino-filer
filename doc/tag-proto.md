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
	"tagid" : <integer>
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
	"taggee" : <integer>,
	"tagger" : <integer>,
	"probability" : <real> // set to values < -1 for find function
}
```

Response
```
{
	"cat" : "tagtag",
	"result" : "OK"/"Tag not found"/"Relationship not found" 
	"relid" : <integer>
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
