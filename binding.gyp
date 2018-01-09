{
  "targets": [
    {
      "target_name": "naaa",
      "sources": [
        "src/naaa.cc",
        "src/Analyser.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ]
    }
  ]
}
