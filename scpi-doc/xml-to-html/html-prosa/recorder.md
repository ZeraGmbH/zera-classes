Commands to record measurement values.

**Schema query 'RECORDER:REC1:EXPORT:JSON?':**
```
{
  "timestamp_first_local": <Local time first record as: yyyy-MM-dd HH:mm:ss:zzz>,
  "timestamp_first_utc": <UTC time first record as: yyyy-MM-dd HH:mm:ss:zzz>,
  "value_sequence": [
    <Name of 1st value in "data" arrays>,
    <Name of 2nd value in "data" arrays>,
    ...
  ],
  "values": [
    {
      "data": [
        <1st value>,
        <2nd value>,
        ...
      ],
      "ts": <ms since first value>
    },
    {
      "data": [
        <1st value>,
        <2nd value>,
        ...
      ],
      "ts": <ms since first value>
    },
    ...
  ]
}
```

