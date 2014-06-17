// for sbt 0.13:
//addSbtPlugin("com.eed3si9n" % "sbt-assembly" % "0.11.2")

// for sbt 0.12:
addSbtPlugin("com.eed3si9n" % "sbt-assembly" % "0.9.2")

//addCompilerPlugin("org.scala-tools.sxr" % "sxr_2.9.0" % "0.2.7")

resolvers += Resolver.url("sbt-plugin-releases_", new URL("http://scalasbt.artifactoryonline.com/scalasbt/sbt-plugin-releases/"))(Resolver.ivyStylePatterns)

addSbtPlugin("com.typesafe.sbt" % "sbt-pgp" % "0.8.1")

addSbtPlugin("com.github.mpeltonen" % "sbt-idea" % "1.6.0")
