{
  "targets": [
    {
      "target_name": "naaa",
      "sources": [
        "src/naaa.cc",
        "src/RealtimeAnalyser.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
