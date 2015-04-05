module LoadConfig

  extend self

  def from_file(filename)
    raw_config = File.read(filename)
    YAML.load(raw_config)
  end

end
