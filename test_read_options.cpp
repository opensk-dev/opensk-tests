# include <options.hpp>
# include <config.hpp>

# include <test/common.hpp>

# include <iostream>
# include <sstream>

BOOST_AUTO_TEST_SUITE(read_options_test_suite)

BOOST_AUTO_TEST_CASE(check_default_behaviour) {
    const char* argv[] = {
        "Z:/invalid_path",
        "--help",
    };

    std::optional<sk::ConfigureOptions> options;

    BOOST_REQUIRE_NO_THROW(options = sk::read_options(std::size(argv), argv));
    BOOST_REQUIRE(options.has_value() == false);
}

std::string run_test(const int argc, const char* argv[], std::optional<sk::ConfigureOptions>& options) {
    // redirect std::cout into output string stream
    auto* old_cout_rdbuf = std::cout.rdbuf();
    std::ostringstream output_buf;
    std::cout.rdbuf(output_buf.rdbuf());
    BOOST_REQUIRE_NO_THROW(options = sk::read_options(argc, argv));
    std::cout.rdbuf(old_cout_rdbuf);
    return output_buf.str();
}

BOOST_AUTO_TEST_CASE(check_option_help) {
    const char* argv[] = {
        "Z:/invalid_path",
        "--help",
    };

    std::optional<sk::ConfigureOptions> options;
    std::string output = run_test(std::size(argv), argv, options);

    BOOST_REQUIRE(options.has_value() == false);

    // check command line descrption
    BOOST_CHECK(output.contains("--help"));
    BOOST_CHECK(output.contains("--version"));
    BOOST_CHECK(output.contains("--config"));

    // check config options description
    BOOST_CHECK(output.contains("data-path"));
    BOOST_CHECK(output.contains("settings-path"));
    BOOST_CHECK(output.contains("saves-path"));
}

BOOST_AUTO_TEST_CASE(check_option_version) {
    const char* argv[] = {
        "Z:/invalid_path",
        "--version",
    };

    std::optional<sk::ConfigureOptions> options;
    std::string output = run_test(std::size(argv), argv, options);

    BOOST_REQUIRE(options.has_value() == false);

    BOOST_CHECK(output.contains("opensk"));
    BOOST_CHECK(output.contains(sk::config::version_string()));
}

// TODO: check invalid cases
// TODO: check --config option
// TODO: check custom config file

BOOST_AUTO_TEST_SUITE_END()
